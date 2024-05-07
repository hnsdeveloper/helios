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

namespace hls {

using uintreg_t = uint64_t;

#define REGISTER_SIZE 8;

#define PAGE_LEVELS 4
#define PAGE_FRAME_SIZE 4096
#define PAGE_FRAME_ALIGNMENT PAGE_FRAME_SIZE
#define PAGE_TABLE_SIZE PAGE_FRAME_SIZE
#define PAGE_TABLE_ENTRY_SIZE 8
#define ENTRIES_PER_TABLE PAGE_FRAME_SIZE / PAGE_TABLE_ENTRY_SIZE
#define VALID_PAGE_BIT uint64_t(1u) << 0
#define READ uint64_t(1u) << 1
#define WRITE uint64_t(1u) << 2
#define EXECUTE uint64_t(1u) << 3
#define USER uint64_t(1u) << 4
#define G_WHAT uint64_t(1u) << 5
#define ACCESS uint64_t(1u) << 6
#define DIRTY uint64_t(1u) << 7

enum class PageLevel : size_t {
    FIRST_VPN = 0,
    KB_VPN = 0,
    MB_VPN = 1,
    GB_VPN = 2,
    TB_VPN = 3,
    LAST_VPN = TB_VPN
};

LKERNELFUN PageLevel next_vpn(PageLevel v);

template <PageLevel...> struct FrameInfo;

template <> struct FrameInfo<PageLevel::KB_VPN> {
    LKERNELRODATA static constexpr size_t size = 4096;
    LKERNELRODATA static constexpr PageLevel page_type = PageLevel::KB_VPN;
    LKERNELRODATA static constexpr size_t alignment = size;
};

template <> struct FrameInfo<PageLevel::MB_VPN> {
    LKERNELRODATA static constexpr size_t size = 512ul * FrameInfo<PageLevel::KB_VPN>::size;
    LKERNELRODATA static constexpr PageLevel page_type = PageLevel::MB_VPN;
    LKERNELRODATA static constexpr size_t alignment = size;
};

template <> struct FrameInfo<PageLevel::GB_VPN> {
    LKERNELRODATA static constexpr size_t size = 512ul * FrameInfo<PageLevel::MB_VPN>::size;
    LKERNELRODATA static constexpr PageLevel page_type = PageLevel::GB_VPN;
    LKERNELRODATA static constexpr size_t alignment = size;
};

template <> struct FrameInfo<PageLevel::TB_VPN> {
    LKERNELRODATA static constexpr size_t size = 512ul * FrameInfo<PageLevel::GB_VPN>::size;
    LKERNELRODATA static constexpr PageLevel page_type = PageLevel::TB_VPN;
    LKERNELRODATA static constexpr size_t alignment = size;
};

template <> struct FrameInfo<> {
    LKERNELFUN static constexpr size_t size_query(PageLevel v) {
        switch (v) {
        case PageLevel::KB_VPN:
            return FrameInfo<PageLevel::KB_VPN>::size;
        case PageLevel::MB_VPN:
            return FrameInfo<PageLevel::MB_VPN>::size;
        case PageLevel::GB_VPN:
            return FrameInfo<PageLevel::GB_VPN>::size;
        case PageLevel::TB_VPN:
            return FrameInfo<PageLevel::TB_VPN>::size;
        }
    };

    LKERNELFUN static constexpr size_t alignment_query(PageLevel v) {
        switch (v) {
        case PageLevel::KB_VPN:
            return FrameInfo<PageLevel::KB_VPN>::alignment;
        case PageLevel::MB_VPN:
            return FrameInfo<PageLevel::MB_VPN>::alignment;
        case PageLevel::GB_VPN:
            return FrameInfo<PageLevel::GB_VPN>::alignment;
        case PageLevel::TB_VPN:
            return FrameInfo<PageLevel::TB_VPN>::alignment;
        }
    }
};

struct PageEntry;
struct PageTable;
template <PageLevel type> struct PageFrame;

struct __attribute__((packed)) PageEntry {
    uint64_t data = 0;

    void erase();

    void set_accessed(bool v);
    void set_dirty(bool v);

    bool is_valid();

    bool is_leaf();
    bool is_table_pointer();

    bool is_writable();
    bool is_readable();
    bool is_executable();

    LKERNELFUN PageTable *as_table_pointer();

    LKERNELFUN void *as_pointer();

    LKERNELFUN void point_to_table(const PageTable *table);

    LKERNELFUN void point_to_frame(const void *frame);

    LKERNELFUN void set_flags(uint64_t flags);

    LKERNELFUN void unset_flags(uint64_t flags);
};

struct __attribute__((packed)) PageTable {
    PageEntry entries[ENTRIES_PER_TABLE];

    LKERNELFUN PageEntry &get_entry(size_t entry_index);
    LKERNELFUN bool is_empty();
};

template <PageLevel v> struct __attribute__((packed)) PageFrame {
    LKERNELRODATA static constexpr size_t s_size = FrameInfo<v>::size;
    LKERNELRODATA static constexpr PageLevel s_type = FrameInfo<v>::page_type;
    LKERNELRODATA static constexpr size_t alignment = FrameInfo<v>::size;

    char data[FrameInfo<v>::size];

    LKERNELFUN PageTable *as_table() {
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