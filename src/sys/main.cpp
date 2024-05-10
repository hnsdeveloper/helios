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
#include "mem/framemanager.hpp"
#include "mem/mmap.hpp"
#include "misc/githash.hpp"
#include "misc/leanmeanparser/optionparser.hpp"
#include "misc/libfdt/libfdt.h"
#include "misc/splash.hpp"
#include "misc/symbols.hpp"
#include "sys/bootdata.hpp"
#include "sys/bootoptions.hpp"
#include "sys/cpu.hpp"
#include "sys/mem.hpp"
#include "sys/print.hpp"
#include "sys/string.hpp"

namespace hls {

void strcprint(const char *str, size_t n) {
    while (n--) {
        opensbi_putchar(*(str++));
    }
}

void die() {
    while (true)
        ;
}

PageTable *init_dirtyfallocator(size_t used, PageTable *tables) {
    static size_t g_usedpages = used;
    static PageTable *g_tables = tables;

    if (used == 0) {
        if (g_usedpages < BOOTPAGES)
            return g_tables + g_usedpages++;
    }

    return nullptr;
}

void *dirtyfallocator() {
    return init_dirtyfallocator(0, nullptr);
}

void nothing_deallocator(void *) {
}

void display_initial_info() {
    // Prints the splash logo
    strprint(splash);
    // Prints copyright notice, the year and the commit which this build was based
    // at.

    kprintln("Copyright (C) {}. Built from {}.", __DATE__ + 7, GIT_HASH);
}

void *get_device_tree_from_options(option::Option *options, option::Option *) {

    if (options[OptionIndex::FDT].count() == 1) {
        char *p = nullptr;
        uintptr_t addr = strtoul(options[OptionIndex::FDT].arg, &p, 16);

        if (addr == 0 && p == nullptr) {
            kprintln("Invalid FDT address. Please reboot and provide a valid one (FDT needed for booting).");
            die();
        }

        return to_ptr(addr);
    } else {
        kprintln("Invalid fdt option.");
    }

    return nullptr;
}

void *mapfdt(void *fdt) {
    PageTable *kptp = get_kernel_pagetable();
    byte *aligned = reinterpret_cast<byte *>(align_back(fdt, PAGE_FRAME_ALIGNMENT));
    kmmap(aligned, aligned, kptp, PageLevel::KB_VPN, READ | ACCESS | DIRTY, dirtyfallocator);
    kmmap(aligned + PAGE_FRAME_SIZE, aligned + PAGE_FRAME_SIZE, kptp, PageLevel::KB_VPN, READ | ACCESS | DIRTY,
          dirtyfallocator);

    size_t fdt_size = fdt_totalsize(fdt);
    size_t needed_size = reinterpret_cast<byte *>(fdt) - aligned + fdt_size;
    size_t needed_pages = needed_size / PAGE_FRAME_SIZE + (needed_size % PAGE_FRAME_SIZE ? 1 : 0);

    kmunmap(aligned, kptp, nothing_deallocator);
    kmunmap(aligned + PAGE_FRAME_SIZE, kptp, nothing_deallocator);

    byte *addr = get_kernel_v_free_address();
    void *retval = addr + (reinterpret_cast<byte *>(fdt) - aligned);
    for (size_t i = 0; i < needed_pages; ++i) {
        kmmap(aligned, addr, kptp, PageLevel::KB_VPN, READ | ACCESS | DIRTY, dirtyfallocator);
        aligned += PAGE_FRAME_SIZE;
        addr += PAGE_FRAME_SIZE;
    }
    set_kernel_v_free_address(addr);
    return retval;
}

void unmap_low_kernel(byte *begin, byte *end) {
    PageTable *kernel_table = get_kernel_pagetable();
    for (auto it = begin; it < end; it += PAGE_FRAME_SIZE) {
        kmunmap(it, kernel_table, nothing_deallocator);
    }
}

[[no_return]] void kernel_main(bootinfo *b_info) {
    display_initial_info();
    int argc = b_info->argc;
    const char **argv = b_info->argv;
    set_kernel_pagetable(b_info->p_kernel_table);
    set_scratch_pagetable(b_info->v_scratch);
    set_kernel_v_free_address(b_info->v_highkernel_end);
    init_dirtyfallocator(b_info->used_bootpages, get_kernel_pagetable());
    unmap_low_kernel(b_info->p_lowkernel_start, b_info->p_lowkernel_end);

    option::Stats stats(usage, argc -= 1, argv += 1);
    option::Option options[stats.options_max], buffer[stats.buffer_max];
    option::Parser parse(usage, argc, argv, options, buffer);

    if (parse.error()) {
        kprintln("Failed to parse boot options.");
        die();
    }

    if (argc == 0 || options[OptionIndex::HELP]) {
        option::printUsage(&strcprint, usage);
        die();
    }

    void *device_tree = get_device_tree_from_options(options, buffer);
    device_tree = mapfdt(device_tree);
    initialize_frame_manager(device_tree, b_info);

    while (true)
        ;
}

}; // namespace hls

extern "C" void _main(hls::bootinfo *info) {
    auto info_cp = *info;
    hls::kernel_main(&info_cp);
}