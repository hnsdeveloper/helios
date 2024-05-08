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

namespace hls {

LKERNELCLSSFUN void PageEntry::point_to_frame(const void *frame) {
    data = to_uintptr_t(frame) >> 2;
    data |= 0x1;
    set_flags(READ);
}

LKERNELCLSSFUN void PageEntry::set_flags(uint64_t flags) {
    data = data | flags;
}

LKERNELCLSSFUN void PageEntry::unset_flags(uint64_t flags) {
    data = (data | flags) ^ flags;
}

LKERNELCLSSFUN void PageEntry::erase() {
    data = 0;
}

LKERNELCLSSFUN bool PageEntry::is_valid() {
    bool result = data & 0x1;

    if (is_writable() && !is_readable())
        result = false;

    return result;
}

LKERNELCLSSFUN void *PageEntry::as_pointer() {
    return to_ptr((data << 2) & 0xFFFFFFFFFFFFF000);
}

LKERNELCLSSFUN bool PageEntry::is_leaf() {
    return is_valid() && (is_readable() || is_executable());
}

LKERNELCLSSFUN bool PageEntry::is_writable() {
    return data & (1u << WRITE);
}
LKERNELCLSSFUN bool PageEntry::is_readable() {
    return data & (1u << READ);
}
LKERNELCLSSFUN bool PageEntry::is_executable() {
    return data & (1u << EXECUTE);
}

LKERNELCLSSFUN PageTable *PageEntry::as_table_pointer() {
    return reinterpret_cast<PageTable *>(as_pointer());
}

LKERNELCLSSFUN void PageEntry::point_to_table(const PageTable *table) {
    data = to_uintptr_t(table) >> 2;
    data |= 0x1;
}

LKERNELCLSSFUN bool PageTable::is_empty() {
    for (size_t i = 0; i < ENTRIES_PER_TABLE; ++i) {
        auto &entry = get_entry(i);
        if (entry.data != 0)
            return false;
    }

    return true;
}

LKERNELCLSSFUN PageEntry &PageTable::get_entry(size_t entry_index) {
    return entries[entry_index];
}

LKERNELFUN PageLevel next_vpn(PageLevel v) {
    if (v == PageLevel::FIRST_VPN)
        return v;
    return static_cast<PageLevel>(static_cast<size_t>(v) - 1);
}

LKERNELFUN void uintprint(uint64_t v) {
    if (v == 0) {
        opensbi_putchar('0');
        return;
    }

    char buffer[256];
    size_t buffer_used = 0;

    for (size_t i = 0; v; ++i, v /= 10, ++buffer_used) {
        buffer[i] = v % 10 + '0';
    }

    for (size_t i = 0; i < buffer_used; ++i) {
        char &c = buffer[buffer_used - i - 1];
        opensbi_putchar(c);
    }
}

LKERNELFUN void hexprint(uintptr_t p) {
    char buffer[16];

    for (size_t i = 0; i < 16; ++i) {
        buffer[i] = p & 0xF;
        p = p >> 4;
    }

    opensbi_putchar('0');
    opensbi_putchar('x');
    for (size_t i = 0; i < 16; ++i) {
        char c = buffer[15 - i];
        if (c <= 9) {
            c = c + '0';
        } else
            c = c - 10 + 'a';
        opensbi_putchar(c);
    }
}

LKERNELFUN void _strprint(const char *str) {
    while (*str) {
        opensbi_putchar(*str);
        ++str;
    }
}

LKERNELFUN void _strprintln(const char *str) {
    _strprint(str);
    opensbi_putchar('\r');
    opensbi_putchar('\n');
}

LKERNELDATA char _0[] = "Table address: ";

LKERNELDATA char _1[] = "\r\ni: ";
LKERNELDATA char _2[] = " entry data: ";
LKERNELDATA char _3[] = " entry pointing address: ";

LKERNELDATA char _4[] = "\r\n";

LKERNELCLSSFUN void PageTable::print_entries() {
    uintptr_t p_address = to_uintptr_t(this);
    _strprint(_0);
    hexprint(p_address);

    for (size_t i = 0; i < ENTRIES_PER_TABLE; ++i) {
        auto &entry = entries[i];
        _strprint(_1);
        uintprint(i);
        _strprint(_2);
        hexprint(entry.data);
        _strprint(_3);
        hexprint(to_uintptr_t(entry.as_pointer()));
        // kprintln("i: {} entry_data: {} {} entry address: {}", i, entry.data, entry.as_pointer(),
        //          reinterpret_cast<const void *>(&entry.data));
        _strprint(_4);
    }

    for (size_t i = 0; i < ENTRIES_PER_TABLE; ++i) {
        auto &entry = entries[i];
        if (entry.is_valid() && !entry.is_leaf())
            entry.as_table_pointer()->print_entries();
    }
}

} // namespace hls
