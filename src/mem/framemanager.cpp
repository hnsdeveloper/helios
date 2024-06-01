/*---------------------------------------------------------------------------------
MIT License

Copyright (c) 2024 Helio Nunes Santos

        Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
        copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
        copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
        AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

---------------------------------------------------------------------------------*/

#include "mem/framemanager.hpp"
#include "mem/mmap.hpp"
#include "misc/libfdt/libfdt.h"
#include "misc/new.hpp"
#include "sys/print.hpp"
#include "ulib/rb_tree.hpp"

#define FRAMEMANAGEMENT_BEGIN 0x1000

namespace hls
{

    template <>
    class Hash<frame_info>
    {
        SET_USING_CLASS(frame_info, type);
        SET_USING_CLASS(uintptr_t, hash_result);

      public:
        hash_result operator()(type_const_reference v) const
        {
            return to_uintptr_t(v.frame_pointer);
        }
    };

    class FrameNodeAllocator
    {
        byte *m_nodes_vaddress_begin = nullptr;
        byte *m_nodes_vaddress_current = nullptr;
        size_t m_node_size;

        FrameNodeAllocator() = default;

        void *m_nodes_list;

      public:
        void *get()
        {
            return remove_begin();
        }

        void release(const void *f)
        {
            insert_begin(const_cast<void *>(f));
        }

        void insert_begin(void *p)
        {
            uintptr_t &v = *reinterpret_cast<uintptr_t *>(p);
            v = to_uintptr_t(m_nodes_list);
            m_nodes_list = p;
        }

        void *remove_begin()
        {
            auto temp = m_nodes_list;
            uintptr_t &p = *reinterpret_cast<uintptr_t *>(m_nodes_list);
            m_nodes_list = to_ptr(p);
            return temp;
        }

        void set_nodes_vaddress_begin(byte *vaddress)
        {
            if (m_nodes_vaddress_begin == nullptr)
            {
                m_nodes_vaddress_begin = vaddress;
                m_nodes_vaddress_current = vaddress;
            }
        }

        void set_frame_node_size(size_t size)
        {
            m_node_size = size;
        }

        size_t map_node_frame(FrameKB *p_frame, FrameKB **frames, size_t count)
        {
            auto vaddress = m_nodes_vaddress_current;
            size_t used_frames = kmmap(p_frame, vaddress, get_kernel_pagetable(), FrameLevel::KB_VPN,
                                       READ | WRITE | ACCESS | DIRTY, frames, count);

            m_nodes_vaddress_current += FrameInfo<FrameLevel::KB_VPN>::s_size;
            for (; vaddress < m_nodes_vaddress_current; vaddress += m_node_size)
            {
                if (vaddress + m_node_size <= m_nodes_vaddress_current)
                    insert_begin(vaddress);
            }
            return used_frames;
        };

        size_t node_count()
        {
            size_t count = 0;
            auto next_nd = [](void *p) {
                uintptr_t &v = *reinterpret_cast<uintptr_t *>(p);
                return to_ptr(v);
            };

            void *nd = m_nodes_list;
            while (nd)
            {
                ++count;
                nd = next_nd(nd);
            }
            return count;
        }

        static FrameNodeAllocator &instance()
        {
            static FrameNodeAllocator s_allocator;
            return s_allocator;
        }
    };

    template <typename T>
    class NodeAllocator
    {
        SET_USING_CLASS(T, type);

      public:
        NodeAllocator()
        {
            m_i = 0;
        }

        size_t m_i;

        template <typename... Args>
        type_ptr create(Args... args)
        {
            type_ptr v = allocate();
            if (v != nullptr)
            {
                new (v) type(hls::forward<Args>(args)...);
            }

            return v;
        }

        void destroy(type_const_ptr p)
        {
            if (p == nullptr)
                return;

            type_ptr p_nc = const_cast<type_ptr>(p);
            (*p_nc).~type();
            deallocate(p_nc);
        }

        type_ptr allocate()
        {
            void *p = FrameNodeAllocator::instance().get();
            return reinterpret_cast<type_ptr>(p);
        }

        void deallocate(type_const_ptr p)
        {
            if (p == nullptr)
                return;
            FrameNodeAllocator::instance().release(p);
        }
    };

    class FrameManager
    {
        using NodeTree = RedBlackTree<frame_info, Hash, LessComparator, NodeAllocator>;

        NodeTree m_used;
        NodeTree m_free;

        size_t m_frame_count = 0;

        static FrameManager &__internal_instance(void *mem, size_t size)
        {
            static FrameManager m(mem, size);
            return m;
        }

        FrameManager(void *mem, size_t size)
        {
            void *p_frame_begin = mem;
            void *p_frame_end = align_back(apply_offset(p_frame_begin, size), FrameKB::s_alignment);

            FrameKB *p_begin = reinterpret_cast<FrameKB *>(align_forward(mem, FrameKB::s_alignment));
            FrameKB *p_end = reinterpret_cast<FrameKB *>(p_frame_end);

            auto &f_node_alloc = FrameNodeAllocator::instance();
            byte *vaddress_begin = nullptr;
            vaddress_begin = vaddress_begin + FRAMEMANAGEMENT_BEGIN;
            f_node_alloc.set_nodes_vaddress_begin(vaddress_begin);
            f_node_alloc.set_frame_node_size(sizeof(NodeTree::node));

            const size_t f_count = static_cast<size_t>(FrameLevel::LAST_VPN);

            FrameKB *frames[f_count];
            for (size_t i = 0; i < f_count; ++i)
            {
                frames[i] = p_begin + 1 + i;
            }

            size_t used_frames = f_node_alloc.map_node_frame(p_begin, frames, f_count);
            p_begin += 1;
            p_begin += used_frames;

            frame_info info;
            info.frame_pointer = p_begin;
            info.frame_count = p_end - p_begin;

            m_frame_count = p_end - p_begin;

            kprintln("Initializing FrameManager with {} frames for a total of {} kb of physical memory.",
                     info.frame_count, (info.frame_count * FrameKB::s_size) / 1024);
            m_free.insert(info);
        }

        void validate_and_fix_node_allocator(size_t minimum_nodes)
        {
            auto &allocator = FrameNodeAllocator::instance();
            if (allocator.node_count() >= minimum_nodes)
            {
                return;
            }

            // We need 1 for mapping and at maximum LAST_VPN frames, given that the root table is always available.
            const size_t f_count = static_cast<size_t>(FrameLevel::LAST_VPN) + 1;
            FrameKB *frames[f_count];

            size_t i = 0;
            auto it = m_free.begin();
            while (i < f_count)
            {
                auto f_info_old = *it;
                auto f_info_new = f_info_old;
                ++it;

                while (f_info_new.frame_count > 0 && i < f_count)
                {
                    frames[i] = f_info_new.frame_pointer;
                    f_info_new.frame_count = f_info_new.frame_count - 1;
                    f_info_new.frame_pointer = f_info_new.frame_pointer + 1;
                    ++i;
                }

                m_free.remove(f_info_old);
                // Should be safe, we have at least one node available
                if (f_info_new.frame_count > 0)
                {
                    m_free.insert(f_info_new);
                }
            }

            auto used = allocator.map_node_frame(frames[0], frames + 1, f_count - 1);
            used = 1;
            for (size_t i = 0; i < f_count; ++i)
            {
                frame_info f_nodes{.frame_pointer = frames[0], .frame_count = 1, .use_count = 1, .flags = 0};
                m_used.insert(f_nodes);
                // This will always be used, given that it is mapping the nodes.
                if (i > 0 && i != used)
                {
                    release_frame(frames[i]);
                }
            }
        }

        bool is_valid_frame(frame_info &info, size_t count, size_t alignment)
        {
            auto ptr = reinterpret_cast<FrameKB *>(align_forward(info.frame_pointer, alignment));
            return ptr + count >= info.frame_pointer && ptr + count <= (info.frame_pointer + info.frame_count);
        }

      public:
#ifdef DEBUG
        void check_count()
        {
            size_t count = 0;
            auto used = m_used.begin();
            auto free = m_free.begin();

            while (used != m_used.end() && free != m_free.end())
            {
                auto fia = *used;
                auto fib = *free;

                if (fia.frame_pointer < fib.frame_pointer)
                {
                    // print_frame_info(fia);
                    count += fia.frame_count;
                    ++used;
                }
                else if (fib.frame_pointer < fia.frame_pointer)
                {
                    // print_frame_info(fib);
                    count += fib.frame_count;
                    ++free;
                }
            }

            while (used != m_used.end())
            {
                // print_frame_info(*used);
                count += used->frame_count;
                ++used;
            }

            while (free != m_free.end())
            {
                // print_frame_info(*free);
                count += free->frame_count;
                ++free;
            }

            if (count != m_frame_count)
            {
                kprintln("Frame count mismatch by {} frames.", m_frame_count - count);
            }
            else
            {
                kprintln("Expected {} frames. Got {} frames.", m_frame_count, count);
            }
        }

        void debug_free()
        {
            kprintln("Free nodes: ");
            for (auto it = m_free.begin(); it != m_free.end(); ++it)
            {
                kprintln("--------------");
                auto &info = *it;
                kspit(info.frame_pointer);
                kspit(info.frame_count);
            }
        };

        void debug_used()
        {
            kprintln("Used nodes: ");
            for (auto it = m_used.begin(); it != m_used.end(); ++it)
            {
                kprintln("--------------");
                auto &info = *it;
                kspit(info.frame_pointer);
                kspit(info.frame_count);
                kspit(info.use_count);
            }
        }

        void print_frame_info(frame_info &f)
        {
            kspit(f.frame_pointer);
            kspit(f.frame_count);
            kspit(f.use_count);
            kspit(f.flags);
        }
#endif

        static FrameManager &instance()
        {
            // TODO: PANIC IF UNITIALIZED
            return __internal_instance(nullptr, 0);
        }

        static bool init(void *mem, size_t mem_size)
        {
            __internal_instance(mem, mem_size);
            return false;
        }

        frame_info *get_frames(size_t count, size_t alignment, uint64_t flags = 0)
        {
            bool is_alignment_valid = alignment % FrameKB::s_alignment == 0;
            if (count == 0 || !is_alignment_valid || m_free.size() == 0)
                return nullptr;
            // Needed when getting a new frame, given that frames can be split and we might have to map more nodes.
            validate_and_fix_node_allocator((size_t)(FrameLevel::LAST_VPN));

            for (auto it = m_free.begin(); it != m_free.end();)
            {
                auto info = *(it++);
                if (is_valid_frame(info, count, alignment))
                {
                    frame_info b{.frame_pointer =
                                     reinterpret_cast<FrameKB *>(align_forward(info.frame_pointer, alignment)),
                                 .frame_count = count,
                                 .use_count = 1,
                                 .flags = flags};

                    frame_info a{.frame_pointer = info.frame_pointer,
                                 // Conversion to suppress compiler warning
                                 .frame_count = (to_uintptr_t(b.frame_pointer) - to_uintptr_t(info.frame_pointer)) /
                                                FrameKB::s_size,
                                 .use_count = 0,
                                 .flags = 0};

                    frame_info c{.frame_pointer = b.frame_pointer + b.frame_count,
                                 .frame_count = info.frame_count - b.frame_count - a.frame_count,
                                 .use_count = 0,
                                 .flags = 0};

                    m_free.remove(info);

                    if (a.size())
                    {
                        if (a.frame_pointer != b.frame_pointer && a.frame_pointer != c.frame_pointer)
                            m_free.insert(a);
                    }

                    if (c.size())
                    {
                        if (c.frame_pointer != b.frame_pointer && c.frame_pointer != a.frame_pointer)
                            m_free.insert(c);
                    }

                    auto nd = m_used.insert(b);
                    auto x = &(nd->get_data());
                    return x;
                }
            }

            return nullptr;
        }

        void release_frame(void *ptr)
        {
            FrameKB *frame = reinterpret_cast<FrameKB *>(ptr);

            auto node_b = m_used.get_node(frame_info{.frame_pointer = frame});
            if (node_b->get_data().use_count != 1)
            {
                node_b->get_data().use_count -= 1;
                return;
            }

            frame_info f = node_b->get_data();
            m_used.remove(f);
            // Either it will be null() (end) or a greater value, so getting the predecessor without checking is ok.
            auto nd = m_free.equal_or_greater(f);
            nd = m_free.get_in_order_predecessor(nd);

            if (nd != nullptr)
            {
                frame_info finfo = nd->get_data();
                frame_info f_old = finfo;
                if (finfo.frame_pointer + finfo.frame_count == f.frame_pointer)
                {
                    m_free.remove(finfo);
                    f_old.frame_count += f.frame_count;
                    f = f_old;
                }
            }

            nd = m_free.get_in_order_successor(nd);

            if (nd != m_free.null())
            {
                frame_info finfo = nd->get_data();
                if (f.frame_pointer + f.frame_count == finfo.frame_pointer)
                {
                    m_free.remove(finfo);
                    f.frame_count += finfo.frame_count;
                }
            }
            f.flags = 0;
            m_free.insert(f);
        }

        size_t get_frame_count()
        {
            return m_frame_count;
        }
    };

    bool is_memory_node(void *fdt, int node)
    {
        const char *memory_string = "memory@";
        int len = 0;
        const char *node_name = fdt_get_name(fdt, node, &len);

        if (len)
        {
            return strncmp(memory_string, node_name, strlen(memory_string)) == 0;
        }

        return false;
    }

    void initialize_frame_manager(void *fdt, bootinfo *b_info)
    {
        byte *mem = b_info->p_kernel_physical_end;
        for (auto node = fdt_first_subnode(fdt, 0); node >= 0; node = fdt_next_subnode(fdt, node))
        {
            if (is_memory_node(fdt, node))
            {
                size_t address_cells = fdt_address_cells(fdt, 0);
                size_t size_cells = fdt_size_cells(fdt, 0);

                int len;
                const struct fdt_property *prop = fdt_get_property(fdt, node, "reg", &len);

                auto get_address = [&](size_t idx) {
                    const char *d = reinterpret_cast<const char *>(prop->data);
                    d += idx * (sizeof(uint32_t) * address_cells + sizeof(uint32_t) * size_cells);
                    uintptr_t p = 0;

                    if (address_cells == 1)
                        p = fdt32_ld(reinterpret_cast<const fdt32_t *>(d));
                    else if (address_cells == 2)
                        p = fdt64_ld(reinterpret_cast<const fdt64_t *>(d));

                    return to_ptr(p);
                };

                auto get_size = [&](size_t idx) {
                    const char *d = reinterpret_cast<const char *>(prop->data);
                    d += idx * (sizeof(uint32_t) * address_cells + sizeof(uint32_t) * size_cells);
                    size_t p = 0;

                    d += address_cells * sizeof(uint32_t);

                    if (size_cells == 1)
                        p = fdt32_ld(reinterpret_cast<const fdt32_t *>(d));
                    else if (size_cells == 2)
                        p = fdt64_ld(reinterpret_cast<const fdt64_t *>(d));

                    return p;
                };

                byte *mem_temp = reinterpret_cast<byte *>(get_address(0));
                size_t mem_size = get_size(0);

                mem_size -= (mem - mem_temp);

                FrameManager::init(mem, mem_size);
                break;
            }
        }

        for (size_t i = 0; i < 100; ++i)
        {
            FrameManager::instance().get_frames(i, i * FrameKB::s_size, 0);
        }
    }

    void *get_frame_management_begin_vaddress()
    {
        return to_ptr(FRAMEMANAGEMENT_BEGIN);
    }
    void *get_frame_management_end_vaddress()
    {
        void *p = get_frame_management_begin_vaddress();
        p = apply_offset(p, FrameManager::instance().get_frame_count() * FrameKB::s_size);
        return align_forward(p, 0x40000000);
    }

    PageTable *init_initfalloc(size_t used, PageTable *tables)
    {
        static size_t g_usedpages = used;
        static PageTable *g_tables = tables;

        if (used == 0)
        {
            if (g_usedpages < BOOTPAGES)
                return g_tables + g_usedpages++;
        }

        return nullptr;
    }

    void *initfalloc()
    {
        return init_initfalloc(0, nullptr);
    }

    void initffree(void *)
    {
        (void)(0);
    }

    const frame_info *framealloc(size_t count, uint64_t flags)
    {
        return FrameManager::instance().get_frames(count, FrameInfo<FrameLevel::KB_VPN>::s_alignment, flags);
    }

    const frame_info *framealloc(uint64_t flags)
    {
        return framealloc(1, flags);
    }

    void framefree(void *ptr)
    {
        FrameManager::instance().release_frame(ptr);
    }

} // namespace hls
