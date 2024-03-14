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
#include "sys/memmap.hpp"
#include "sys/paging.hpp"
#include "sys/panic.hpp"
#include "sys/print.hpp"

extern "C" void _die();

namespace hls {

void check_system_capabilities() {
  uint64_t misa_val = read_csr(MCSR::misa).get_value();
  auto calc_shift = [](char c) { return c - 'a'; };

  misa_val = 0;

  if (!(misa_val & 0x1ul << calc_shift('i')))
    kprint("I extension required to run HeliOS.\r\n");
  if (!(misa_val & 0x1ul << calc_shift('m')))
    kprint("M extension required to run HeliOS.\r\n");
  if (!(misa_val & 0x1ul << calc_shift('a')))
    kprint("A extension required to run HeliOS.\r\n");
  if (!(misa_val & 0x1ul << calc_shift('f')))
    kprint("F extension required to run HeliOS.\r\n");
  if (!(misa_val & 0x1ul << calc_shift('d')))
    kprint("D extension required to run HeliOS.\r\n");
  if (!(misa_val & 0x1ul << calc_shift('c')))
    kprint("C extension required to run HeliOS.\r\n");
};

void test_func() { PANIC("This is a kernel panic test!"); }

void main(int argc, const char **argv) {

  // Stops compiler complains for now
  for (int i = 0; i < argc; ++i) {
    void *x = argv + i;
  }

  setup_printing();
  strprintln("Booting HeliOS!");

  test_func();

  strprintln("Checking system capabilities!");
  check_system_capabilities();

  strprintln("Setting up paging system.");
  setup_paging();

  strprintln("Mapping kernel memory.");
  setup_kernel_memory_mapping();
}

} // namespace hls

extern "C" void bootmain(int argc, const char **argv) { hls::main(argc, argv); }