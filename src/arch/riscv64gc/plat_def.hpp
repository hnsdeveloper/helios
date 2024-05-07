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

#include "misc/types.hpp"
#include "sys/mem.hpp"

namespace hls {

using uintreg_t = uint64_t;

const size_t REGISTER_SIZE = 8;

// TODO: CHECK WHERE THESE SYMBOLS ARE BEING USED
const size_t PAGE_LEVELS = 4;
const size_t PAGE_FRAME_SIZE = 4096;
const size_t PAGE_FRAME_ALIGNMENT = PAGE_FRAME_SIZE;
const size_t PAGE_TABLE_SIZE = PAGE_FRAME_SIZE;
const size_t PAGE_TABLE_ENTRY_SIZE = 8;
const size_t ENTRIES_PER_TABLE = PAGE_FRAME_SIZE / PAGE_TABLE_ENTRY_SIZE;
const size_t VALID_PAGE_BIT = 0;
const size_t READ_BIT = 1;
const size_t WRITE_BIT = 2;
const size_t EXECUTE_BIT = 3;

#define HIGHEST_ADDRESS (void *)(0xFFFFFFFFFFFF)

enum class PageLevel : size_t {
    FIRST_VPN = 0,
    KB_VPN = 0,
    MB_VPN = 1,
    GB_VPN = 2,
    TB_VPN = 3,
    LAST_VPN = TB_VPN
};

PageLevel next_vpn(PageLevel v);

template <PageLevel...> struct FrameInfo;

template <> struct FrameInfo<PageLevel::KB_VPN> {
    static constexpr size_t size = 4096;
    static constexpr PageLevel page_type = PageLevel::KB_VPN;
    static constexpr size_t alignment = size;
};

template <> struct FrameInfo<PageLevel::MB_VPN> {
    static constexpr size_t size = 512ul * FrameInfo<PageLevel::KB_VPN>::size;
    static constexpr PageLevel page_type = PageLevel::MB_VPN;
    static constexpr size_t alignment = size;
};

template <> struct FrameInfo<PageLevel::GB_VPN> {
    static constexpr size_t size = 512ul * FrameInfo<PageLevel::MB_VPN>::size;
    static constexpr PageLevel page_type = PageLevel::GB_VPN;
    static constexpr size_t alignment = size;
};

template <> struct FrameInfo<PageLevel::TB_VPN> {
    static constexpr size_t size = 512ul * FrameInfo<PageLevel::GB_VPN>::size;
    static constexpr PageLevel page_type = PageLevel::TB_VPN;
    static constexpr size_t alignment = size;
};

template <> struct FrameInfo<> {
    static constexpr size_t size_query(PageLevel v) {
        switch (v) {
        case PageLevel::KB_VPN:
            return FrameInfo<PageLevel::KB_VPN>::size;
        case PageLevel::MB_VPN:
            return FrameInfo<PageLevel::MB_VPN>::size;
        case PageLevel::GB_VPN:
            return FrameInfo<PageLevel::GB_VPN>::size;
        case PageLevel::TB_VPN:
            return FrameInfo<PageLevel::TB_VPN>::size;
        default: {
            PANIC("Invalid page level.");
        }
        }
    };

    static constexpr size_t alignment_query(PageLevel v) {
        switch (v) {
        case PageLevel::KB_VPN:
            return FrameInfo<PageLevel::KB_VPN>::alignment;
        case PageLevel::MB_VPN:
            return FrameInfo<PageLevel::MB_VPN>::alignment;
        case PageLevel::GB_VPN:
            return FrameInfo<PageLevel::GB_VPN>::alignment;
        case PageLevel::TB_VPN:
            return FrameInfo<PageLevel::TB_VPN>::alignment;
        default: {
            PANIC("Invalid page level.");
        }
        }
    }
};

struct PageEntry;
struct PageTable;
template <PageLevel type> struct PageFrame;

struct __attribute__((packed)) PageEntry {
    uint64_t data = 0;

    void erase();

    void make_writable(bool v);
    void make_readable(bool v);
    void make_executable(bool v);

    void set_accessed(bool v);
    void set_dirty(bool v);

    bool is_valid();

    bool is_leaf();
    bool is_table_pointer();

    bool is_writable();
    bool is_readable();
    bool is_executable();

    void *as_pointer();

    PageTable *as_table_pointer();

    void point_to_table(PageTable *table);

    template <PageLevel v> void point_to_frame(const PageFrame<v> *frame) {
        using frame_t = PageFrame<v>;
        if (is_aligned(frame, alignof(frame_t))) {
            data = to_uintptr_t(frame) >> 2;
            data |= 0x1;
            make_readable(true);
        }
    }

    template <PageLevel v> PageFrame<v> *as_frame_pointer() {
        return reinterpret_cast<PageFrame<v> *>(as_pointer());
    };
};

struct __attribute__((packed)) PageTable {
    PageEntry entries[ENTRIES_PER_TABLE];

    PageEntry &get_entry(size_t entry_index);
    bool is_empty();

    void print_entries();
};

template <PageLevel v> struct __attribute__((packed)) PageFrame {
    static constexpr size_t s_size = FrameInfo<v>::size;
    static constexpr PageLevel s_type = FrameInfo<v>::page_type;
    static constexpr size_t alignment = FrameInfo<v>::size;

    char data[FrameInfo<v>::size];

    PageTable *as_table() {
        if constexpr (FrameInfo<v>::page_type == PageLevel::KB_VPN)
            return reinterpret_cast<PageTable *>(this);

        return nullptr;
    }
};

using PageKB = PageFrame<PageLevel::KB_VPN>;
using PageMB = PageFrame<PageLevel::MB_VPN>;
using PageGB = PageFrame<PageLevel::GB_VPN>;
using PageTB = PageFrame<PageLevel::TB_VPN>;

using GranularPage = PageKB;

} // namespace hls

#endif