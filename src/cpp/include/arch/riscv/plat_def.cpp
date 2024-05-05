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

#include "include/arch/riscv/plat_def.hpp"

using namespace hls;

void PageEntry::erase() {
    data = 0;
}

void PageEntry::make_writable(bool v) {
    if (v) {
        make_readable(v);
        data = data | (1u << WRITE_BIT);
    } else {
        data = (data | (1u << WRITE_BIT)) | (1u << WRITE_BIT);
    }
}

void PageEntry::make_readable(bool v) {
    if (v) {
        data = data | (1u << READ_BIT);
    } else {
        make_writable(v);
        make_executable(v);
        data = (data | (1u << READ_BIT)) | (1u << READ_BIT);
    }
}

void PageEntry::make_executable(bool v) {
    if (v) {
        make_readable(v);
        data = data | (1u << EXECUTE_BIT);
    } else {
        data = (data | (1u << EXECUTE_BIT)) | (1u << EXECUTE_BIT);
    }
}

bool PageEntry::is_valid() {
    bool result = data & 0x1;

    if (is_writable() && !is_readable())
        result = false;

    return result;
}

void *PageEntry::as_pointer() {
    return to_ptr((data << 2) & 0x00fffffffffff000);
}

bool PageEntry::is_leaf() {
    return is_valid() && (is_readable() || is_executable());
}

bool PageEntry::is_writable() {
    return data & (1u << WRITE_BIT);
}
bool PageEntry::is_readable() {
    return data & (1u << READ_BIT);
}
bool PageEntry::is_executable() {
    return data & (1u << EXECUTE_BIT);
}

PageTable *PageEntry::as_table_pointer() {
    return reinterpret_cast<PageTable *>(to_ptr((data >> 10) << 12));
}

void PageEntry::point_to_table(PageTable *table) {
    if (is_aligned(table, alignof(PageTable))) {
        data = to_uintptr_t(table) >> 2;
        data |= 0x1;
    }
}

PageEntry &PageTable::get_entry(size_t entry_index) {
    return entries[entry_index];
}

size_t get_page_entry_index(void *v_address, PageLevel vpn) {
    size_t result = 0;
    size_t vpn_idx = static_cast<size_t>(vpn);
    auto addr = to_uintptr_t(v_address);
    result = (addr >> (12 + (vpn_idx * 9))) & 0x1FF;

    return result;
}

PageLevel hls::next_vpn(PageLevel v) {
    return static_cast<PageLevel>(static_cast<size_t>(v) - 1);
}

void PageTable::print_entries() {
    for (size_t i = 0; i < ENTRIES_PER_TABLE; ++i) {
        auto &entry = entries[i];
        kprintln("i: {} entry_data: {} {} entry address: {}", i, entry.data, entry.as_pointer(),
                 reinterpret_cast<const void *>(&entry.data));
    }
}