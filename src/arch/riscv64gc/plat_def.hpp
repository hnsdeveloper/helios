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

#define REGISTER_SIZE 8;

#define PAGE_LEVELS 4
#define PAGE_FRAME_SIZE 4096
#define PAGE_FRAME_ALIGNMENT PAGE_FRAME_SIZE
#define PAGE_TABLE_SIZE PAGE_FRAME_SIZE
#define PAGE_TABLE_ENTRY_SIZE 8
#define ENTRIES_PER_TABLE PAGE_FRAME_SIZE / PAGE_TABLE_ENTRY_SIZE
#define VALID uint64_t(1u) << 0
#define READ uint64_t(1u) << 1
#define WRITE uint64_t(1u) << 2
#define EXECUTE uint64_t(1u) << 3
#define USER uint64_t(1u) << 4
#define G_WHAT uint64_t(1u) << 5
#define ACCESS uint64_t(1u) << 6
#define DIRTY uint64_t(1u) << 7

    using uintreg_t = uint64_t;
    using max_align_t = void *;

    enum class FrameLevel : size_t
    {
        FIRST_VPN = 0,
        KB_VPN = 0,
        MB_VPN = 1,
        GB_VPN = 2,
        TB_VPN = 3,
        LAST_VPN = TB_VPN,
        INVALID
    };

    FrameLevel next_vpn(FrameLevel v);
    void _flush_tlb();

    template <FrameLevel P>
    struct FrameInfo
    {
        static constexpr size_t s_size =
            ENTRIES_PER_TABLE * FrameInfo<static_cast<FrameLevel>(static_cast<size_t>(P) - 1)>::s_size;
        static constexpr size_t s_alignment = s_size;
        static constexpr FrameLevel s_level = P;

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
    struct FrameInfo<FrameLevel::INVALID>;

    template <>
    struct FrameInfo<FrameLevel::FIRST_VPN>
    {
        static constexpr size_t s_size = PAGE_FRAME_SIZE;
        static constexpr size_t s_alignment = s_size;
        static constexpr FrameLevel s_level = FrameLevel::FIRST_VPN;

        static bool can_fit(size_t bytes)
        {
            if (bytes <= s_size)
            {
                return true;
            }

            return false;
        }
    };

    size_t get_frame_size(FrameLevel lvl);
    size_t get_frame_alignment(FrameLevel lvl);

    FrameLevel get_fit_level(size_t bytes);

    struct TableEntry;
    struct PageTable;
    template <FrameLevel type>
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

        void set_flags(uint64_t flags);

        void unset_flags(uint64_t flags);
    };

    template <FrameLevel v>
    struct __attribute__((packed)) PageFrame
    {
        static constexpr size_t s_size = FrameInfo<v>::s_size;
        static constexpr size_t s_alignment = FrameInfo<v>::s_alignment;
        static constexpr FrameLevel s_level = FrameInfo<v>::s_level;

        char data[FrameInfo<v>::s_size];
    };

    template <>
    struct __attribute__((packed)) PageFrame<FrameLevel::KB_VPN>
    {
        static constexpr size_t s_size = FrameInfo<FrameLevel::KB_VPN>::s_size;
        static constexpr size_t s_alignment = FrameInfo<FrameLevel::KB_VPN>::s_alignment;
        static constexpr FrameLevel s_level = FrameInfo<FrameLevel::KB_VPN>::s_level;

        char data[FrameInfo<FrameLevel::KB_VPN>::s_size];

        PageTable *as_table()
        {
            return reinterpret_cast<PageTable *>(this);
        }
    };

    struct __attribute__((packed)) PageTable : public PageFrame<FrameLevel::KB_VPN>
    {
        static constexpr size_t entries_on_table = ENTRIES_PER_TABLE;
        TableEntry *entries();
        TableEntry &get_entry(size_t entry_index);
        void print_entries();
        bool is_empty();
    };

    using FrameKB = PageFrame<FrameLevel::KB_VPN>;
    static_assert(sizeof(FrameKB) == FrameInfo<FrameLevel::KB_VPN>::s_size);
    static_assert(sizeof(PageTable) == sizeof(FrameKB));
    using FrameMB = PageFrame<FrameLevel::MB_VPN>;
    static_assert(sizeof(FrameMB) == FrameInfo<FrameLevel::MB_VPN>::s_size);
    using FrameGB = PageFrame<FrameLevel::GB_VPN>;
    static_assert(sizeof(FrameGB) == FrameInfo<FrameLevel::GB_VPN>::s_size);
    using FrameTB = PageFrame<FrameLevel::TB_VPN>;
    static_assert(sizeof(FrameTB) == FrameInfo<FrameLevel::TB_VPN>::s_size);

    using GranularPage = FrameKB;

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
