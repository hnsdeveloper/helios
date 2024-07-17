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

#ifndef _PLAT_DEF_H_
#define _PLAT_DEF_H_

#include "misc/macros.hpp"
#include "misc/types.hpp"

namespace hls
{
    constexpr uint64_t REGISTER_SIZE = 8;
    constexpr uint64_t PAGE_LEVELS = 4;
    constexpr uint64_t PAGE_FRAME_SIZE = 4096;
    constexpr uint64_t PAGE_FRAME_ALIGNMENT = PAGE_FRAME_SIZE;
    constexpr uint64_t PAGE_TABLE_SIZE = PAGE_FRAME_SIZE;
    constexpr uint64_t PAGE_TABLE_ENTRY_SIZE = 8;
    constexpr uint64_t ENTRIES_PER_TABLE = PAGE_FRAME_SIZE / PAGE_TABLE_ENTRY_SIZE;
    constexpr uint64_t VALID = uint64_t(1u) << 0;
    constexpr uint64_t READ = uint64_t(1u) << 1;
    constexpr uint64_t WRITE = uint64_t(1u) << 2;
    constexpr uint64_t EXECUTE = uint64_t(1u) << 3;
    constexpr uint64_t USER = uint64_t(1u) << 4;
    constexpr uint64_t G_WHAT = uint64_t(1u) << 5;
    constexpr uint64_t ACCESS = uint64_t(1u) << 6;
    constexpr uint64_t DIRTY = uint64_t(1u) << 7;

    using uintreg_t = uint64_t;
    using max_align_t = void *;

    enum class FrameOrder : size_t
    {
        LOWEST_ORDER = 0,
        FIRST_ORDER = 0,
        SECOND_ORDER = 1,
        THIRD_ORDER = 2,
        FOURTH_ORDER = 3,
#ifdef SV39
        HIGHEST_ORDER = THIRD_ORDER,
#elif SV48
        HIGHEST_ORDER = FOURTH_ORDER,
#endif
        INVALID
    };

    void kinit_putchar(char c);
    FrameOrder next_vpn(FrameOrder v);
    void _flush_tlb();

    template <FrameOrder P>
    struct FrameInfo
    {
        static constexpr size_t s_size =
            ENTRIES_PER_TABLE * FrameInfo<static_cast<FrameOrder>(static_cast<size_t>(P) - 1)>::s_size;
        static constexpr size_t s_alignment = s_size;
        static constexpr FrameOrder s_level = P;

        static bool can_fit(size_t bytes)
        {
            if (bytes <= s_size)
            {
                return true;
            }

            return false;
        }
    };

    template <>
    struct FrameInfo<FrameOrder::INVALID>;

    template <>
    struct FrameInfo<FrameOrder::LOWEST_ORDER>
    {
        static constexpr size_t s_size = PAGE_FRAME_SIZE;
        static constexpr size_t s_alignment = s_size;
        static constexpr FrameOrder s_level = FrameOrder::LOWEST_ORDER;

        static bool can_fit(size_t bytes)
        {
            if (bytes <= s_size)
            {
                return true;
            }

            return false;
        }
    };

    size_t get_frame_size(FrameOrder lvl);
    size_t get_frame_alignment(FrameOrder lvl);

    FrameOrder get_fit_level(size_t bytes);

    struct TableEntry;
    struct PageTable;
    template <FrameOrder type>
    struct PageFrame;

    struct __attribute__((packed)) TableEntry
    {
        uint64_t data = 0;

        void erase();

        bool is_valid();

        bool is_leaf();
        bool is_table_pointer();

        bool is_writable();
        bool is_readable();
        bool is_executable();

        PageTable *as_table_pointer();

        void *as_pointer();

        void point_to_table(const PageTable *table);

        void point_to_frame(const void *frame);

        void set_system_flags(uint64_t flags);

        void unset_system_flags(uint64_t flags);

        uint64_t get_platform_flagmask() const;

        uint64_t get_system_flagmask() const;
    };

    template <FrameOrder v>
    struct __attribute__((packed)) PageFrame
    {
        static constexpr size_t s_size = FrameInfo<v>::s_size;
        static constexpr size_t s_alignment = FrameInfo<v>::s_alignment;
        static constexpr FrameOrder s_level = FrameInfo<v>::s_level;

        char data[FrameInfo<v>::s_size];
    };

    template <>
    struct __attribute__((packed)) PageFrame<FrameOrder::FIRST_ORDER>
    {
        static constexpr size_t s_size = FrameInfo<FrameOrder::FIRST_ORDER>::s_size;
        static constexpr size_t s_alignment = FrameInfo<FrameOrder::FIRST_ORDER>::s_alignment;
        static constexpr FrameOrder s_level = FrameInfo<FrameOrder::FIRST_ORDER>::s_level;

        char data[FrameInfo<FrameOrder::FIRST_ORDER>::s_size];

        PageTable *as_table()
        {
            return reinterpret_cast<PageTable *>(this);
        }
    };

    struct __attribute__((packed)) PageTable : public PageFrame<FrameOrder::FIRST_ORDER>
    {
        static constexpr size_t entries_on_table = ENTRIES_PER_TABLE;
        TableEntry *entries();
        TableEntry &get_entry(size_t entry_index);
        void *compose_address_with_entry(void *vaddress, size_t entry_idx, FrameOrder order);
        void print_entries();
        bool is_empty();
    };

    using FrameKB = PageFrame<FrameOrder::FIRST_ORDER>;
    static_assert(sizeof(FrameKB) == FrameInfo<FrameOrder::FIRST_ORDER>::s_size);
    static_assert(sizeof(PageTable) == sizeof(FrameKB));
    using FrameMB = PageFrame<FrameOrder::SECOND_ORDER>;
    static_assert(sizeof(FrameMB) == FrameInfo<FrameOrder::SECOND_ORDER>::s_size);
    using FrameGB = PageFrame<FrameOrder::THIRD_ORDER>;
    static_assert(sizeof(FrameGB) == FrameInfo<FrameOrder::THIRD_ORDER>::s_size);

#ifdef SV48
    using FrameTB = PageFrame<FrameOrder::FOURTH_ORDER>;
    static_assert(sizeof(FrameTB) == FrameInfo<FrameOrder::FOURTH_ORDER>::s_size);
#endif

    struct __attribute__((packed)) _reg_as_data
    {
        uint64_t x0;
        uint64_t x1;
        uint64_t x2;
        uint64_t x3;
        uint64_t x4;
        uint64_t x5;
        uint64_t x6;
        uint64_t x7;
        uint64_t x8;
        uint64_t x9;
        uint64_t x10;
        uint64_t x11;
        uint64_t x12;
        uint64_t x13;
        uint64_t x14;
        uint64_t x15;
        uint64_t x16;
        uint64_t x17;
        uint64_t x18;
        uint64_t x19;
        uint64_t x20;
        uint64_t x21;
        uint64_t x22;
        uint64_t x23;
        uint64_t x24;
        uint64_t x25;
        uint64_t x26;
        uint64_t x27;
        uint64_t x28;
        uint64_t x29;
        uint64_t x30;
        uint64_t x31;
        uint64_t pc;
    };

    struct registers
    {
        static constexpr size_t register_count = 32;
        union {
            _reg_as_data data;
            uint64_t array[32];
        } reg;
    };

} // namespace hls

#endif
