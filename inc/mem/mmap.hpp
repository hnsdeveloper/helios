#include "arch/riscv64/plat_def.hpp"
#include "klibc/kstddef.h"
#include "klibc/kstdint.h"
#include "mem/bumpallocator.hpp"
#include "mem/nodeallocator.hpp"
#include "misc/macros.hpp"
#include "sys/bootdata.hpp"
#include "sys/cpu.hpp"
#include "ulib/rb_tree.hpp"
#include "ulib/result.hpp"
#include "ulib/singleton.hpp"

namespace hls
{
    constexpr uint64_t VM_VALID_FLAG = 0x1;
    constexpr uint64_t VM_READ_FLAG = 0x1 << 1;
    constexpr uint64_t VM_WRITE_FLAG = 0x1 << 2;
    constexpr uint64_t VM_EXECUTE_FLAG = 0x1 << 3;
    constexpr uint64_t VM_ACCESS_FLAG = 0x1 << 4;
    constexpr uint64_t VM_DIRTY_FLAG = 0x1 << 5;

    class MemMapInfo
    {
        FrameOrder m_order;
        void *m_paddress;
        void *m_vaddress;
        uint64_t m_flags;

        bool inside_range(const void *a, size_t size, const void *b)
        {
            byte *_a = as_byte_ptr(a);
            byte *_b = as_byte_ptr(b);

            return (_b < (_a + size)) && (_b >= _a);
        }

      public:
        MemMapInfo(FrameOrder f_order, void *paddress, void *vaddress, uint64_t flags)
            : m_order(f_order), m_paddress(paddress), m_vaddress(vaddress), m_flags(flags)
        {
        }

        void *get_vaddress() const
        {
            return m_vaddress;
        }

        void *get_paddress() const
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

    class VMMap : public Singleton<VMMap>
    {
        PageTable *m_p_root_table;
        PageTable *m_v_scratch_table;

        PageTable *get_scratch_table();
        FrameKB *physical_frame_to_scratch_frame(FrameKB *frame);
        Pair<FrameOrder, PageTable *> table_walk(const void *vaddress, PageTable *table, FrameOrder order);
        VMMap(PageTable *table, PageTable *scratch_table);

      public:
        Result<MemMapInfo> map_memory(void *paddress, void *vaddress, FrameOrder order, uint64_t flags);
        Result<MemMapInfo> map_first_fit(void *paddress, FrameOrder order, uint64_t flags);
        void unmap_memory(void *v_address);
        Result<MemMapInfo> get_mapping_data(const void *vaddress) const;
        bool is_address_mapped(const void *vaddress);
        bool is_valid_virtual_address(const void *vaddress);

        friend class Singleton<VMMap>;
    };
} // namespace hls
