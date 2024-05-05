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
#include "misc/githash.hpp"
#include "misc/libfdt/libfdt.h"
#include "misc/splash.hpp"
#include "sys/bootoptions.hpp"
#include "sys/opensbi.hpp"
#include "sys/panic.hpp"
#include "sys/print.hpp"
#include "sys/string.hpp"
#include "sys/traphandler/traphandler.hpp"
#include "sys/virtualmemory/common.hpp"
#include "sys/virtualmemory/kmalloc.hpp"
#include "sys/virtualmemory/memmap.hpp"
#include "sys/virtualmemory/paging.hpp"

namespace hls {

/**
 * @brief Displays HeliOS logo and copyright notice.
 *
 */
void display_initial_info() {
    // Prints the splash logo
    strprint(splash);
    // Prints copyright notice, the year and the commit which this build was based
    // at.
    kprintln("Copyright (C) {}. Built from {}.", __DATE__ + 7, GIT_HASH);
}

void *get_device_tree_from_options(option::Option *options, option::Option *) {
    kspit(options[OptionIndex::FDT].count());

    if (options[OptionIndex::FDT].count() == 1) {
        char *p = nullptr;
        uintptr_t addr = strtoul(options[OptionIndex::FDT].arg, &p, 16);

        if (addr == 0 && p == nullptr) {
            kprintln("Invalid FDT address. Please reboot and provide a valid one.");
            die();
        }

        return to_ptr(addr);
    } else {
        kprintln("Invalid fdt option.");
    }

    return nullptr;
}

// TODO: IMPLEMENT GETTING CPU ID
size_t cpu_id() {
    return 0;
}

/**
 * @brief Main function. Initialize the required kernel subsystems.
 *
 * @param argc Argument count
 * @param argv Array of string arguments
 */
[[noreturn]] void main(int argc, const char **argv) {
    if (cpu_id() == 0) {
        setup_printing();
        display_initial_info();

        argc = argc - (argc > 0);
        argv = argv + (argc > 0);
        option::Stats stats(usage, argc, argv);
        option::Option options[stats.options_max], buffer[stats.buffer_max];
        option::Parser parse(usage, argc, argv, options, buffer);

        kspit(stats.options_max);
        kspit(stats.buffer_max);

        if (parse.error()) {
            kprintln("Failed to parse boot options.");
            die();
        }

        if (argc == 0 || options[OptionIndex::HELP]) {
            option::printUsage(&strcprint, usage);
            die();
        }

        void *device_tree = get_device_tree_from_options(options, buffer);

        kprintln("Loading flattened device tree at {}", device_tree);

        strprintln("Setting up pageframe manager.");
        setup_page_frame_manager(device_tree);

        strprintln("Mapping kernel memory.");
        setup_kernel_memory_mapping();

        strprintln("Setting up memory allocators.");
        initialize_kmalloc();
        // initialize_vmalloc();

        strprintln("Building reserved virtual addresses map.");
        // build_reserved_addresses_map(device_tree);

        strprintln("Setting up kernel trap handling.");
        setup_trap_handling();

        enable_address_translation(kernel_page_table);

        while (true)
            ;

        // TODO:
        // Solve TODOs. Refactor whole code. Write documentation.
        // Initialize scheduler
        // First userland application (included in the kernel itself). Should be
        // shell with a simple echo command Develop file system module Load first
        // userland application from filesystem Enable swapping Improve shell, allow
        // it to issue other commands Implement libc (probably use newlib) Have
        // simple commands running simultaneously Detect devices Implement drivers
        // Implement multiple terminals
    }

    while (true)
        ;
}

} // namespace hls

/**
 * @brief Accessible symbol from assembly. Used to call the real main function.
 *
 * @param argc Argument count
 * @param argv Array of string arguments
 */
extern "C" [[noreturn]] void bootmain(int argc, const char **argv) {
    hls::main(argc, argv);
}