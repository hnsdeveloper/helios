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
#include "misc/macros.hpp"
#include "misc/symbols.hpp"
#include "sys/mem.hpp"
#include "sys/opensbi.hpp"
#include "sys/print.hpp"

#ifndef BOOT_PAGE_COUNT
#define BOOT_PAGE_COUNT 32
#endif

using namespace hls;

LKERNELBSS alignas(4096) PageKB INITIAL_FRAMES[BOOT_PAGE_COUNT];
LKERNELRODATA char HERE1[] = "HERE1";
LKERNELRODATA char HERE2[] = "HERE2";
LKERNELRODATA char HERE3[] = "HERE3";
LKERNELRODATA char HERE4[] = "HERE4";
LKERNELRODATA char HERE5[] = "HERE5";
LKERNELRODATA char HERE6[] = "HERE6";

#define nvpn(__v) __v == PageLevel::KB_VPN ? PageLevel::KB_VPN : static_cast<PageLevel>(static_cast<size_t>(__v) - 1)

LKERNELFUN size_t pe_idx(const void *vaddress, PageLevel v) {
    size_t vpn_idx = static_cast<size_t>(v);
    uintptr_t idx = to_uintptr_t(vaddress) >> 12;
    return (idx >> (vpn_idx * 9)) & 0x1FF;
}

LKERNELFUN void twlk(const void *vaddress, PageTable **table, PageLevel *lvl) {
    if (table == nullptr || lvl == nullptr)
        return;

    PageLevel l = *lvl;
    PageTable *t = *table;
    size_t idx = pe_idx(vaddress, l);
    auto &entry = t->entries[idx];
    if (entry.data & VALID) {
        if (!(entry.data & (READ | WRITE | EXECUTE))) {
            *table = reinterpret_cast<PageTable *>((entry.data >> 10) << 12);
            *lvl = nvpn(*lvl);
        }
    }
}

LKERNELFUN void init_f_alloc() {
    memset(INITIAL_FRAMES, 0, sizeof(INITIAL_FRAMES));
}

LKERNELFUN void *f_alloc() {
    LKERNELDATA static size_t i = 0;

    GranularPage *p = INITIAL_FRAMES;
    if (i < BOOT_PAGE_COUNT)
        return p + i++;
    return nullptr;
}

LKERNELFUN bool bkmmap(const void *paddress, const void *vaddress, PageTable *table, const PageLevel p_lvl,
                       uint64_t flags) {

    if (table == nullptr)
        return false;

    PageLevel c_lvl = PageLevel::LAST_VPN;
    PageLevel expected = nvpn(c_lvl);
    PageTable *t = table;

    while (c_lvl != p_lvl) {
        twlk(vaddress, &t, &c_lvl);
        if (c_lvl == expected) {
            expected = nvpn(expected);
            continue;
        }

        size_t idx = pe_idx(vaddress, c_lvl);
        auto &entry = t->entries[idx];

        if (entry.data & (READ | WRITE | EXECUTE))
            return false;

        if (!(entry.data & VALID)) {
            void *frame = f_alloc();
            memset(frame, 0, PAGE_FRAME_SIZE);
            entry.data = ((reinterpret_cast<uint64_t>(frame) >> 12) << 10);
            entry.data = entry.data | VALID;
        }

        c_lvl = nvpn(c_lvl);
        expected = nvpn(expected);
        t = reinterpret_cast<PageTable *>(to_ptr((entry.data >> 10) << 12));
    }

    size_t lvl_entry_idx = pe_idx(vaddress, c_lvl);
    auto &entry = t->entries[lvl_entry_idx];
    entry.data = (to_uintptr_t(paddress) >> 12) << 10;
    entry.data = entry.data | VALID | flags;

    return true;
}

LKERNELFUN void map_high_kernel(PageTable *kernel_table) {
    byte *_k_physical = &_kload_begin;

    for (auto i = &_text_begin; i != &_text_end; i += PAGE_FRAME_SIZE, _k_physical += PAGE_FRAME_SIZE) {
        bkmmap(_k_physical, i, kernel_table, PageLevel::FIRST_VPN, READ | EXECUTE | ACCESS | DIRTY);
    }

    for (auto i = &_rodata_begin; i != &_rodata_end; i += PAGE_FRAME_SIZE, _k_physical += PAGE_FRAME_SIZE) {
        bkmmap(_k_physical, i, kernel_table, PageLevel::FIRST_VPN, READ | ACCESS | DIRTY);
    }

    // DATA, BSS and STACK are all READ and WRITE
    for (auto i = &_data_begin; i != &_stack_end; i += PAGE_FRAME_SIZE, _k_physical += PAGE_FRAME_SIZE) {
        bkmmap(_k_physical, i, kernel_table, PageLevel::FIRST_VPN, READ | WRITE | ACCESS | DIRTY);
    }
}

LKERNELFUN void identity_map(PageTable *kernel_table) {
    for (auto i = &_load_address; i != &_kload_begin; i += PAGE_FRAME_SIZE) {
        bkmmap(i, i, kernel_table, PageLevel::FIRST_VPN, READ | WRITE | EXECUTE | ACCESS | DIRTY);
    }
}

LKERNELFUN void map_args(PageTable *kernel_table, int argc, char **argv) {
    for (size_t i = 0; i < argc; ++i) {
        ptrprint(*argv);
    }
}

extern "C" LKERNELFUN void *bootmain(int argc, char **argv) {
    init_f_alloc();
    PageTable *kernel_table = reinterpret_cast<PageTable *>(f_alloc());
    map_high_kernel(kernel_table);
    identity_map(kernel_table);
    return kernel_table;
}