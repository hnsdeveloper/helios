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

void TableEntry::point_to_frame(const void *frame) {
    data = to_uintptr_t(frame) >> 2;
    data |= 0x1;
    set_flags(READ);
}

void TableEntry::set_flags(uint64_t flags) {
    data = data | flags;
}

void TableEntry::unset_flags(uint64_t flags) {
    data = (data | flags) ^ flags;
}

void TableEntry::erase() {
    data = 0;
}

bool TableEntry::is_valid() {
    bool result = data & 0x1;
    return result;
}

void *TableEntry::as_pointer() {
    return to_ptr((data << 2) & 0xFFFFFFFFFFFFF000);
}

bool TableEntry::is_leaf() {
    return is_valid() && (is_readable() || is_executable());
}

bool TableEntry::is_writable() {
    return data & (1u << WRITE);
}
bool TableEntry::is_readable() {
    return data & (1u << READ);
}
bool TableEntry::is_executable() {
    return data & (1u << EXECUTE);
}

PageTable *TableEntry::as_table_pointer() {
    return reinterpret_cast<PageTable *>(as_pointer());
}

void TableEntry::point_to_table(const PageTable *table) {
    data = to_uintptr_t(table) >> 2;
    data |= 0x1;
}

TableEntry *PageTable::entries() {
    return reinterpret_cast<TableEntry *>(data);
}

bool PageTable::is_empty() {
    for (size_t i = 0; i < ENTRIES_PER_TABLE; ++i) {
        auto &entry = get_entry(i);
        if (entry.data != 0)
            return false;
    }

    return true;
}

TableEntry &PageTable::get_entry(size_t entry_index) {
    return entries()[entry_index];
}

FrameLevel next_vpn(FrameLevel v) {
    if (v == FrameLevel::FIRST_VPN)
        return v;
    return static_cast<FrameLevel>(static_cast<size_t>(v) - 1);
}

size_t get_frame_size(FrameLevel lvl) {
    switch (lvl) {
    case FrameLevel::KB_VPN:
        return FrameInfo<FrameLevel::KB_VPN>::s_size;
    case FrameLevel::MB_VPN:
        return FrameInfo<FrameLevel::MB_VPN>::s_size;
    case FrameLevel::GB_VPN:
        return FrameInfo<FrameLevel::GB_VPN>::s_size;
    case FrameLevel::TB_VPN:
        return FrameInfo<FrameLevel::TB_VPN>::s_size;
    default:
        return 0;
    }
}

size_t get_frame_alignment(FrameLevel lvl) {
    return get_frame_size(lvl);
}

FrameLevel get_fit_level(size_t bytes) {
    FrameLevel lvl = FrameLevel::LAST_VPN;

    while (lvl != FrameLevel::FIRST_VPN) {
        if (bytes <= get_frame_size(lvl) && bytes > get_frame_size(next_vpn(lvl)))
            return next_vpn(lvl);
        lvl = next_vpn(lvl);
    }

    return lvl;
}

void PageTable::print_entries() {
    for (size_t i = 0; i < ENTRIES_PER_TABLE; ++i) {
        auto &entry = entries()[i];
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
