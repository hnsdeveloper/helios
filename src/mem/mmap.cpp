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

#include "mem/mmap.hpp"
#include "mem/framemanager.hpp"
#include "sys/mem.hpp"
#include "sys/opensbi.hpp"

namespace hls {

LKERNELRODATA const char HERE[] = "Here";
LKERNELRODATA const char HERE2[] = "Here2";
LKERNELRODATA const char NEW_LINE2[] = "\r\n";

LKERNELFUN void strprint2(const char *str) {
    while (*str) {
        opensbi_putchar(*str);
        ++str;
    }
}

LKERNELFUN void strprintln2(const char *str) {
    strprint2(str);
    strprint2(NEW_LINE2);
}

LKERNELFUN size_t get_page_entry_index(const void *vaddress, PageLevel v) {
    size_t vpn_idx = static_cast<size_t>(v);
    uintptr_t idx = to_uintptr_t(vaddress) >> 12;
    return (idx >> (vpn_idx * 9)) & 0x1FF;
}

LKERNELFUN uintptr_t get_vaddress_offset(const void *vaddress) {
    uintptr_t p = to_uintptr_t(vaddress);
    return p & 0xFFF;
}

LKERNELFUN void walk_table(const void *vaddress, PageTable **table, PageLevel *lvl) {
    if (table == nullptr || lvl == nullptr)
        return;

    PageLevel l = *lvl;
    PageTable *t = *table;

    size_t idx = get_page_entry_index(vaddress, l);
    auto &entry = t->get_entry(idx);

    if (entry.is_valid()) {
        if (!entry.is_leaf()) {
            *table = entry.as_table_pointer();
            *lvl = next_vpn(*lvl);
        }
    }
}

LKERNELFUN void *v_to_p(const void *vaddress, PageTable *table = nullptr) {
    if (table != nullptr) {

        constexpr size_t p_lvl_count = (size_t)(PageLevel::LAST_VPN);
        PageLevel lvl = PageLevel::LAST_VPN;
        for (size_t i = 0; i < p_lvl_count; ++i) {
            // Will walk the table as much as it can.
            // If it is a superpage, then it won't walk more, and then we only apply the offset to the address
            walk_table(vaddress, &table, &lvl);
        }

        auto &entry = table->get_entry(get_page_entry_index(vaddress, lvl));

        if (entry.is_valid() && entry.is_leaf()) {
            return apply_offset(entry.as_pointer(), get_vaddress_offset(vaddress));
        }
    }
    return nullptr;
}

LKERNELFUN bool kmmap(const void *paddress, const void *vaddress, PageTable *table, const PageLevel p_lvl,
                      uint64_t flags, frame_fn f_src) {

    if (table == nullptr)
        return false;

    PageLevel c_lvl = PageLevel::LAST_VPN;
    PageLevel expected = next_vpn(c_lvl);
    PageTable *t = table;

    while (c_lvl != p_lvl) {
        walk_table(vaddress, &t, &c_lvl);
        if (c_lvl == expected) {
            expected = next_vpn(expected);
            continue;
        }

        size_t idx = get_page_entry_index(vaddress, c_lvl);
        auto &entry = t->get_entry(idx);

        if (entry.is_leaf())
            return false;

        if (!entry.is_valid()) {
            void *frame = f_src();
            memset(frame, 0, PAGE_FRAME_SIZE);
            entry.point_to_table(reinterpret_cast<PageTable *>(frame));
        }

        c_lvl = next_vpn(c_lvl);
        expected = next_vpn(expected);
        t = entry.as_table_pointer();
    }

    size_t lvl_entry_idx = get_page_entry_index(vaddress, c_lvl);
    auto &entry = t->get_entry(lvl_entry_idx);
    entry.point_to_frame(paddress);
    entry.set_flags(flags);

    return true;
}

} // namespace hls