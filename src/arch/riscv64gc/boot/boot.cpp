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
#include "sys/string.hpp"

#ifndef BOOTPAGES
#define BOOTPAGES 32
#endif

#ifndef ARGPAGES
#define ARGPAGES 2
#endif

using namespace hls;

LKERNELBSS alignas(PAGE_FRAME_SIZE) PageKB INITIAL_FRAMES[BOOTPAGES];
LKERNELBSS alignas(PAGE_FRAME_SIZE) byte ARGCV[PAGE_FRAME_SIZE * ARGPAGES];

LKERNELRODATA const char NEEDPAGES[] = "Not enough pages. Please, compile kernel with higher BOOTPAGES option.";
LKERNELRODATA const char NEEDARGCV[] =
    "Not enough pages for arguments. Please, compile kernel with higher ARGPAGES option.";
LKERNELDATA byte *kvaddress = reinterpret_cast<byte *>(uintptr_t(0) - uintptr_t(0x40000000));

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
    if (i < BOOTPAGES)
        return p + i++;

    strprintln(NEEDPAGES);
    while (true)
        ;

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

    kvaddress = &_text_begin;
    for (; kvaddress != &_text_end; kvaddress += PAGE_FRAME_SIZE, _k_physical += PAGE_FRAME_SIZE) {
        bkmmap(_k_physical, kvaddress, kernel_table, PageLevel::FIRST_VPN, READ | EXECUTE | ACCESS | DIRTY);
    }
    kvaddress = &_rodata_begin;
    for (; kvaddress != &_rodata_end; kvaddress += PAGE_FRAME_SIZE, _k_physical += PAGE_FRAME_SIZE) {
        bkmmap(_k_physical, kvaddress, kernel_table, PageLevel::FIRST_VPN, READ | ACCESS | DIRTY);
    }

    kvaddress = &_data_begin;
    // DATA, BSS and STACK are all READ and WRITE
    for (; kvaddress != &_stack_end; kvaddress += PAGE_FRAME_SIZE, _k_physical += PAGE_FRAME_SIZE) {
        bkmmap(_k_physical, kvaddress, kernel_table, PageLevel::FIRST_VPN, READ | WRITE | ACCESS | DIRTY);
    }
}

LKERNELFUN void map_page_tables(PageTable *kernel_table) {
    for (size_t i = 0; i < BOOTPAGES; ++i) {
        bkmmap(INITIAL_FRAMES + i, kvaddress, kernel_table, PageLevel::KB_VPN, READ | WRITE | ACCESS | DIRTY);
        kvaddress += PAGE_FRAME_SIZE;
    }
}

LKERNELFUN void identity_map(PageTable *kernel_table) {
    for (auto i = &_load_address; i != &_kload_begin; i += PAGE_FRAME_SIZE) {
        bkmmap(i, i, kernel_table, PageLevel::FIRST_VPN, READ | WRITE | EXECUTE | ACCESS | DIRTY);
    }
}

LKERNELFUN const char **map_args(PageTable *kernel_table, int argc, const char **argv) {
    size_t consumed_bytes = 0;
    byte *c = ARGCV;
    const char *nargv[argc];
    for (size_t i = 0; i < argc; ++i) {
        const char *str = argv[i];
        // Memory length, not string length
        size_t len = strlen(str) + 1;
        memcpy(c, str, len);
        nargv[i] = reinterpret_cast<const char *>(kvaddress + consumed_bytes);
        consumed_bytes += len;
        c += len;
        if (consumed_bytes > sizeof(ARGCV)) {
            strprintln(NEEDARGCV);
            while (true)
                ;
        }
    }

    if (consumed_bytes + sizeof(nargv) > sizeof(ARGCV)) {
        strprintln(NEEDARGCV);
        while (true)
            ;
    }
    memcpy(c, &nargv, sizeof(nargv));

    auto old_kv = kvaddress;
    for (size_t i = 0; i < ARGPAGES; ++i) {
        bkmmap(ARGCV + i, kvaddress, kernel_table, PageLevel::KB_VPN, READ | ACCESS | DIRTY);
        kvaddress += PAGE_FRAME_SIZE;
    }

    return reinterpret_cast<const char **>(old_kv + consumed_bytes);
}

extern "C" LKERNELFUN void bootmain(int argc, const char **argv, PageTable **kptp, PageTable **kptv,
                                    const char ***argvnew, byte **unmapped) {
    init_f_alloc();
    PageTable *kernel_table = reinterpret_cast<PageTable *>(f_alloc());
    map_high_kernel(kernel_table);
    identity_map(kernel_table);

    *kptv = reinterpret_cast<PageTable *>(kvaddress);
    map_page_tables(kernel_table);

    *argvnew = map_args(kernel_table, argc, argv);
    *kptp = kernel_table;
    *unmapped = kvaddress;
}