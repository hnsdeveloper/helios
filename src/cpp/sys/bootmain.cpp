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

#include "include/arch/riscv/plat_def.h"
#include "misc/githash.hpp"
#include "misc/libfdt/libfdt.h"
#include "misc/splash.hpp"
#include "sys/opensbi.hpp"
#include "sys/panic.hpp"
#include "sys/print.hpp"
#include "sys/string.hpp"
#include "sys/traphandler/traphandler.hpp"
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

Result<void *> get_fdt(int argc, const char **argv) {
  // TODO: FOR NOW WE USING THE SECOND ARGUMENT AS THE ADDRESS OF THE FDT

  kdebug(argv[1]);

  void *fdt = to_ptr(hex_to_uint(argv[1]).get_value());
  kdebug(fdt);
  int fdt_check_result = fdt_check_header(fdt);

  kdebug(fdt_check_result);

  if (fdt_check_result != 0)
    return error<void *>(Error::CORRUPTED_DATA_STRUCTURE);

  return value(fdt);
}

size_t cpu_id() { return 0; }

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

    // Reading MISA register is not supported anymore, given that now we are
    // running on top of OpenSBI on S-Mode
    // strprintln("Checking system capabilities!");
    // check_system_capabilities();

    void *device_tree = get_fdt(argc, argv).get_value();

    strprintln("Setting up pageframe manager.");
    setup_page_frame_manager(device_tree);

    strprintln("Mapping kernel memory.");
    void *kernel_page_table = setup_kernel_memory_mapping(device_tree);

    strprintln("Setting up kernel trap handling.");
    setup_trap_handling();

    asm("add a0, x0, %0;"
        "srli a0, a0, 12;"
        "li a1, 0x9000000000000000;"
        "or a0, a0, a1;"
        "csrrw a0, satp, a0;"
        :
        : "r"(kernel_page_table));

    while (true) {
      char *z = (char *)to_ptr(0xc0000000);
      z += 0x8000;
      char c = *z;
    };

    // initialize_kmalloc();

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
}

} // namespace hls

/**
 * @brief Accessible symbol from assembly. Used to call the real main function/
 *
 * @param argc Argument count
 * @param argv Array of string arguments
 */
extern "C" [[noreturn]] void bootmain(int argc, const char **argv) {
  hls::main(argc, argv);
}