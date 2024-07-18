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

#include "arch/riscv64gc/plat_def.hpp"
#include "mem/mmap.hpp"
#include "sys/mem.hpp"
#include "sys/print.hpp"

namespace hls
{

    struct sbi_result
    {
        uint64_t error;
        uint64_t value;
    };

    sbi_result sbi_call(uint64_t extension, uint64_t function_id, uint64_t arg0, uint64_t arg1, uint64_t arg2,
                        uint64_t arg3, uint64_t arg4, uint64_t arg5)
    {
        register uint64_t a0 asm("a0") = arg0;
        register uint64_t a1 asm("a1") = arg1;
        register uint64_t a2 asm("a2") = arg2;
        register uint64_t a3 asm("a3") = arg3;
        register uint64_t a4 asm("a4") = arg4;
        register uint64_t a5 asm("a5") = arg5;
        register uint64_t a6 asm("a6") = function_id;
        register uint64_t a7 asm("a7") = extension;
        asm volatile("ecall" : "+r"(a0), "+r"(a1) : "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a6), "r"(a7) : "memory");

        return {.error = a0, .value = a1};
    }

    void kinit_putchar(char c)
    {
        sbi_call(0x1u, 0x0, c, 0, 0, 0, 0, 0);
    }

    void kinit_settimer(uint64_t ticks)
    {
        sbi_call(0x0, 0, ticks, 0, 0, 0, 0, 0);
    }

    void _flush_tlb()
    {
        asm volatile("sfence.vma x0, x0");
    }

    void TableEntry::point_to_frame(const void *frame)
    {
        data = to_uintptr_t(frame) >> 2;
        data = data | VALID;
        set_system_flags(VM_READ_FLAG);
    }

    void TableEntry::set_system_flags(uint64_t flags)
    {
        if (flags & VM_VALID_FLAG)
            data = data | VALID;
        if (flags & VM_READ_FLAG)
            data = data | READ;
        if (flags & VM_DIRTY_FLAG)
            data = data | DIRTY;
        if (flags & VM_WRITE_FLAG)
            data = data | WRITE;
        if (flags & VM_ACCESS_FLAG)
            data = data | ACCESS;
        if (flags & VM_EXECUTE_FLAG)
            data = data | EXECUTE;
    }

    void TableEntry::unset_system_flags(uint64_t flags)
    {
        if (flags & VM_VALID_FLAG)
            data = (data | VALID) ^ VALID;
        if (flags & VM_READ_FLAG)
            data = (data | READ) ^ READ;
        if (flags & VM_DIRTY_FLAG)
            data = (data | DIRTY) ^ DIRTY;
        if (flags & VM_WRITE_FLAG)
            data = (data | WRITE) ^ WRITE;
        if (flags & VM_ACCESS_FLAG)
            data = (data | ACCESS) ^ ACCESS;
        if (flags & VM_EXECUTE_FLAG)
            data = (data | EXECUTE) ^ EXECUTE;
    }

    void TableEntry::erase()
    {
        data = 0;
    }

    bool TableEntry::is_valid()
    {
        bool result = data & 0x1;
        return result;
    }

    void *TableEntry::as_pointer()
    {
        return to_ptr((data << 2) & 0xFFFFFFFFFFFFF000);
    }

    bool TableEntry::is_leaf()
    {
        return is_valid() && (is_readable() || is_executable());
    }

    bool TableEntry::is_writable()
    {
        return (data & WRITE) != 0;
    }
    bool TableEntry::is_readable()
    {
        return (data & READ) != 0;
    }
    bool TableEntry::is_executable()
    {
        return (data & EXECUTE) != 0;
    }

    uint64_t TableEntry::get_platform_flagmask() const
    {
        uint64_t flags = 0;
        // TODO: Implement
        return flags;
    }

    uint64_t TableEntry::get_system_flagmask() const
    {
        uint64_t flags = 0;
        if (data & READ)
            flags = flags | VM_READ_FLAG;
        if (data & WRITE)
            flags = flags | VM_WRITE_FLAG;
        if (data & EXECUTE)
            flags = flags | VM_EXECUTE_FLAG;
        if (data & ACCESS)
            flags = flags | VM_ACCESS_FLAG;
        if (data & DIRTY)
            flags = flags | VM_DIRTY_FLAG;
        return flags;
    }

    PageTable *TableEntry::as_table_pointer()
    {
        return reinterpret_cast<PageTable *>(as_pointer());
    }

    void *PageTable::compose_address_with_entry(void *vaddress, size_t entry_idx, FrameOrder order)
    {
        uintptr_t p = to_uintptr_t(vaddress);
        uintptr_t vpn_idx = static_cast<size_t>(order);
        p = p | ((entry_idx & 0x1FF) << ((vpn_idx * 9) + 12));
        if (p & (1ull << 47))
            p = p | 0xFFFF000000000000ULL;
        return to_ptr(p);
    }

    void TableEntry::point_to_table(const PageTable *table)
    {
        data = to_uintptr_t(table) >> 2;
        data |= VALID;
    }

    TableEntry *PageTable::entries()
    {
        return reinterpret_cast<TableEntry *>(data);
    }

    bool PageTable::is_empty()
    {
        for (size_t i = 0; i < ENTRIES_PER_TABLE; ++i)
        {
            auto &entry = get_entry(i);
            if (entry.data != 0)
                return false;
        }

        return true;
    }

    TableEntry &PageTable::get_entry(size_t entry_index)
    {
        return entries()[entry_index];
    }

    FrameOrder next_vpn(FrameOrder v)
    {
        if (v == FrameOrder::LOWEST_ORDER)
            return v;
        return static_cast<FrameOrder>(static_cast<size_t>(v) - 1);
    }

    size_t get_frame_size(FrameOrder lvl)
    {
        switch (lvl)
        {
        case FrameOrder::FIRST_ORDER:
            return FrameInfo<FrameOrder::FIRST_ORDER>::s_size;
        case FrameOrder::SECOND_ORDER:
            return FrameInfo<FrameOrder::SECOND_ORDER>::s_size;
        case FrameOrder::THIRD_ORDER:
            return FrameInfo<FrameOrder::THIRD_ORDER>::s_size;
#ifdef SV48
        case FrameOrder::FOURTH_ORDER:
            return FrameInfo<FrameOrder::FOURTH_ORDER>::s_size;
#endif
        default:
            return 0;
        }
    }

    size_t get_frame_alignment(FrameOrder lvl)
    {
        return get_frame_size(lvl);
    }

    FrameOrder get_fit_level(size_t bytes)
    {
        FrameOrder lvl = FrameOrder::HIGHEST_ORDER;

        while (lvl != FrameOrder::LOWEST_ORDER)
        {
            if (bytes <= get_frame_size(lvl) && bytes > get_frame_size(next_vpn(lvl)))
                return next_vpn(lvl);
            lvl = next_vpn(lvl);
        }

        return lvl;
    }

    void PageTable::print_entries()
    {
        for (size_t i = 0; i < ENTRIES_PER_TABLE; ++i)
        {
            auto &entry = entries()[i];
            if (entry.is_valid())
            {
                kprintln("Entry {}. Pointed address: {}. Is leaf? {}.", i, entry.as_pointer(), entry.is_leaf());
            }
        }
        /*
        for (size_t i = 0; i < ENTRIES_PER_TABLE; ++i) {
            auto &entry = entries[i];
            if (entry.is_valid() && !entry.is_leaf())
                entry.as_table_pointer()->print_entries();
        }
        */
    }

} // namespace hls
