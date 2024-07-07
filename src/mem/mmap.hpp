#include "arch/riscv64gc/plat_def.hpp"
#include "mem/bumpallocator.hpp"
#include "mem/nodeallocator.hpp"
#include "misc/macros.hpp"
#include "misc/types.hpp"
#include "sys/bootdata.hpp"
#include "sys/cpu.hpp"
#include "ulib/rb_tree.hpp"
#include "ulib/singleton.hpp"

namespace hls
{

    class MemMapInfo
    {
        FrameOrder m_order;
        const void *m_paddress;
        const void *m_vaddress;
        uint64_t m_flags;

        bool inside_range(const void *a, size_t size, const void *b)
        {
            byte *_a = as_byte_ptr(a);
            byte *_b = as_byte_ptr(b);

            return (_b < (_a + size)) && (_b >= _a);
        }

      public:
        MemMapInfo(FrameOrder f_order, const void *paddress, const void *vaddress, uint64_t flags)
            : m_order(f_order), m_paddress(paddress), m_vaddress(vaddress), m_flags(flags)
        {
        }

        const void *get_vaddress() const
        {
            return m_vaddress;
        }

        const void *get_paddress() const
        {
            return m_paddress;
        }

        size_t get_size() const
        {
            return get_frame_size(m_order);
        }

        FrameOrder get_frame_order() const
        {
            return m_order;
        }

        bool overlaps(const MemMapInfo &other)
        {
            return inside_range(m_vaddress, get_frame_size(m_order), other.m_vaddress) ||
                   inside_range(other.m_vaddress, get_frame_size(other.m_order), m_vaddress);
        }

        uint64_t get_flags() const
        {
            return m_flags;
        }
    };

    template <>
    class Hash<MemMapInfo>
    {
        SET_USING_CLASS(MemMapInfo, type);
        SET_USING_CLASS(uintptr_t, hash_result);

      public:
        hash_result operator()(type_const_reference v) const
        {
            return to_uintptr_t(v.get_vaddress());
        }
    };

    class VMMap : public Singleton<VMMap>
    {
        using tree = RedBlackTree<MemMapInfo, Hash, LessComparator, NodeAllocator>;
        BumpAllocator m_bump_allocator;
        tree m_memmap_info_tree;
        PageTable *m_root_table;
        void *m_min_alloc_address;
        void *m_max_alloc_address;

        bool is_valid_virtual_address(const void *addr);
        void map_to_table(const void *p_address, const void *v_address);
        void translate_to_page(const MemMapInfo &m_map);

      public:
        VMMap(PageTable *table, void *min_map_addr, void *max_map_addr);

        MemMapInfo *map_memory(const void *p_address, const void *v_address, FrameOrder order, uint64_t flags);
        void unmap_memory(const void *v_address);

        /*
        MemMapInfo* map_memory_first_fit(const void* paddress, const void* vaddress, FrameOrder f_order, uint64_t
        flags)
        {
            return nullptr;
        }
        */
    };

    void *v_to_p(const void *vaddress, PageTable *table);

    bool kmmap(const void *paddress, const void *vaddress, PageTable *table, FrameOrder p_lvl, uint64_t flags,
               frame_fn f_src);

    void kmunmap(const void *vaddress, PageTable *start_table, frame_rls_fn rls_fn);

    uintptr_t get_vaddress_offset(const void *vaddress);

    void set_scratch_pagetable(PageTable *vpaddress);
    PageTable *get_scratch_pagetable();

    void set_kernel_pagetable(PageTable *paddress);
    PageTable *get_kernel_pagetable();

    void set_kernel_v_free_address(byte *vaddress);
    byte *get_kernel_v_free_address();

    void setup_kernel_memory_map(bootinfo *b_info);

    void unmap_low_kernel(byte *begin, byte *end);

} // namespace hls
