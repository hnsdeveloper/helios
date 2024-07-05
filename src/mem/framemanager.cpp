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
#include "sys/devicetree.hpp"
#include "sys/print.hpp"
#include "ulib/pair.hpp"
#include "ulib/rb_tree.hpp"

#define FRAMEMANAGEMENT_BEGIN 0x1000

namespace hls
{
    FrameData::FrameData(FrameKB *f_ptr, size_t frame_count, size_t flags)
        : m_frame_pointer(f_ptr), m_frame_count(frame_count), m_use_count(1), m_flags(flags)
    {
    }

    FrameData::FrameData(FrameKB *f_ptr, size_t frame_count) : FrameData(f_ptr, frame_count, 0)
    {
    }

    FrameData::UserData &FrameData::get_userdata()
    {
        const auto &as_const = *this;
        return const_cast<UserData &>(as_const.get_userdata());
    }

    FrameData::FrameData(const FrameData &other)
    {
        m_frame_pointer = other.m_frame_pointer;
        m_frame_count = other.m_frame_count;
        m_use_count = other.m_use_count;
        m_flags = other.m_flags;
        m_userdata = other.m_userdata;
    }

    FrameData::FrameData(FrameData &&other)
    {
        m_frame_pointer = hls::move(other.m_frame_pointer);
        m_frame_count = hls::move(other.m_frame_count);
        m_use_count = hls::move(other.m_use_count);
        m_flags = hls::move(other.m_flags);
        m_userdata = hls::move(other.m_userdata);
    }

    const FrameData::UserData &FrameData::get_userdata() const
    {
        return m_userdata;
    }

    void FrameData::set_flags(uint64_t flags)
    {
        m_flags = flags;
    }

    size_t FrameData::get_flags() const
    {
        return m_flags;
    }

    size_t FrameData::get_frame_count() const
    {
        return m_frame_count;
    }

    size_t FrameData::size() const
    {
        return m_frame_count * FrameKB::s_size;
    }

    FrameKB *FrameData::get_frame_pointer() const
    {
        return m_frame_pointer;
    }

    FrameData &FrameData::shrink_begin(size_t frames)
    {
        if (frames <= m_frame_count)
        {
            m_frame_count = m_frame_count - frames;
        }
        return *this;
    }

    FrameData &FrameData::shrink_end(size_t frames)
    {
        if (frames <= m_frame_count)
        {
            m_frame_count = m_frame_count - frames;
        }

        return *this;
    }

    FrameManager::FrameManager(const Pair<void *, size_t> &mem_info)
        : m_bump_allocator(sizeof(tree::node)), m_used_frames(m_bump_allocator), m_free_frames(m_bump_allocator)
    {
        FrameKB *mem_init = reinterpret_cast<FrameKB *>(align_forward(mem_info.first, alignof(FrameKB)));
        FrameKB *mem_end =
            reinterpret_cast<FrameKB *>(align_back(apply_offset(mem_init, mem_info.second), alignof(FrameKB)));
        m_frame_count = (size_t)(mem_end - mem_init);
        m_free_frames.insert({mem_init, m_frame_count, 0});
        kprintln("Initializing FrameManager with {} frames for a total of {}KiB of memory.", m_frame_count,
                 m_frame_count * FrameKB::s_size / 1024);
    }

    FrameData *FrameManager::get_frames(size_t count, uint64_t flags)
    {
        if (count >= m_frame_count)
        {
            // TODO: Handle freeing memory.
            return nullptr;
        }

        for (auto it = m_free_frames.begin(); it != m_free_frames.end(); ++it)
        {
            if (it->get_frame_count() >= count)
            {
                auto temp_a = *it;
                auto temp_b = temp_a;
                m_free_frames.remove(*it);
                if (count > temp_a.get_frame_count())
                    temp_a.shrink_end(temp_a.get_frame_count() - count);
                temp_b.shrink_begin(count);
                if (temp_b.get_frame_count() > 0)
                    m_free_frames.insert(hls::move(temp_b));
                temp_a.set_flags(flags);
                auto it = m_used_frames.insert(hls::move(temp_a));
                return &(it->get_data());
            }
        }
        return nullptr;
    }

    void FrameManager::release_frames(void *frame_pointer)
    {
        (void)(frame_pointer);
    }

    bool is_memory_node(void *fdt, int node)
    {
        const char *memory_string = "memory@";
        int len = 0;
        const char *node_name = fdt_get_name(fdt, node, &len);
        if (len)
            return strncmp(memory_string, node_name, strlen(memory_string)) == 0;

        return false;
    }

    Pair<void *, size_t> get_available_ram(void *fdt, bootinfo *b_info)
    {
        byte *mem = b_info->p_kernel_physical_end;
        size_t mem_size = 0;
        for (auto node = fdt_first_subnode(fdt, 0); node >= 0; node = fdt_next_subnode(fdt, node))
        {
            if (is_memory_node(fdt, node))
            {
                // A memory node has to be a child of the root node, thus we can use node 0 directly to
                // get address_cells and size_cells. These two properties are inherited from the parent
                // according to the Device Tree specification.
                const fdt32_t *a_cells =
                    reinterpret_cast<const fdt32_t *>(fdt_getprop(fdt, 0, "#address-cells", nullptr));
                const fdt32_t *s_cells = reinterpret_cast<const fdt32_t *>(fdt_getprop(fdt, 0, "#size-cells", nullptr));
                auto reg = read_fdt_prop_reg_prop(fdt, node, a_cells, s_cells);
                byte *mem_temp = as_byte_ptr(reg.mem_address);
                mem_size = reg.mem_size;
                mem_size = mem_size - (mem - mem_temp);
                break;
            }
        }
        return {mem, mem_size};
    }

    void initialize_frame_manager(void *fdt, bootinfo *b_info)
    {
        Pair<void *, size_t> mem_info = get_available_ram(fdt, b_info);
        FrameManager::initialize_global_instance(mem_info);
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
} // namespace hls
