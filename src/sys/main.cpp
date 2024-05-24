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
#include "misc/splash.hpp"
#include "misc/symbols.hpp"
#include "sys/bootdata.hpp"
#include "sys/bootoptions.hpp"
#include "sys/cpu.hpp"
#include "sys/devicetree.hpp"
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

void display_initial_info() {
    // Prints the splash logo
    strprint(splash);
    // Prints copyright notice, the year and the commit which this build was based
    // at.

    kprintln("Copyright (C) {}. Built from {}.", __DATE__ + 7, GIT_HASH);
}

__attribute__((noreturn)) void kernel_main(bootinfo *b_info) {
    display_initial_info();

    setup_kernel_memory_map(b_info);

    init_initfalloc(b_info->used_bootpages, get_kernel_pagetable());

    unmap_low_kernel(b_info->p_lowkernel_start, b_info->p_lowkernel_end);

    option::Stats stats(usage, b_info->argc - 1, b_info->argv + 1);
    option::Option options[stats.options_max], buffer[stats.buffer_max];
    option::Parser parse(usage, b_info->argc - 1, b_info->argv + 1, options, buffer);

    if (parse.error()) {
        kprintln("Failed to parse boot options.");
        die();
    }

    if (b_info->argc == 1 || options[OptionIndex::HELP]) {
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