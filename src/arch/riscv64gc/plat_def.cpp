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
#include "sys/mem.hpp"
#include "sys/opensbi.hpp"
#include "sys/print.hpp"

namespace hls {

void _flush_tlb() {
    asm volatile("sfence.vma x0, x0");
}

void PageEntry::point_to_frame(const void *frame) {
    data = to_uintptr_t(frame) >> 2;
    data |= 0x1;
    set_flags(READ);
}

void PageEntry::set_flags(uint64_t flags) {
    data = data | flags;
}

void PageEntry::unset_flags(uint64_t flags) {
    data = (data | flags) ^ flags;
}

void PageEntry::erase() {
    data = 0;
}

bool PageEntry::is_valid() {
    bool result = data & 0x1;
    return result;
}

void *PageEntry::as_pointer() {
    return to_ptr((data << 2) & 0xFFFFFFFFFFFFF000);
}

bool PageEntry::is_leaf() {
    return is_valid() && (is_readable() || is_executable());
}

bool PageEntry::is_writable() {
    return data & (1u << WRITE);
}
bool PageEntry::is_readable() {
    return data & (1u << READ);
}
bool PageEntry::is_executable() {
    return data & (1u << EXECUTE);
}

PageTable *PageEntry::as_table_pointer() {
    return reinterpret_cast<PageTable *>(as_pointer());
}

void PageEntry::point_to_table(const PageTable *table) {
    data = to_uintptr_t(table) >> 2;
    data |= 0x1;
}

bool PageTable::is_empty() {
    for (size_t i = 0; i < ENTRIES_PER_TABLE; ++i) {
        auto &entry = get_entry(i);
        if (entry.data != 0)
            return false;
    }

    return true;
}

PageEntry &PageTable::get_entry(size_t entry_index) {
    return entries[entry_index];
}

PageLevel next_vpn(PageLevel v) {
    if (v == PageLevel::FIRST_VPN)
        return v;
    return static_cast<PageLevel>(static_cast<size_t>(v) - 1);
}

void PageTable::print_entries() {
    for (size_t i = 0; i < ENTRIES_PER_TABLE; ++i) {
        auto &entry = entries[i];
        if (entry.is_valid()) {
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
