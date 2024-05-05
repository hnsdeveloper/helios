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

#include "sys/virtualmemory/common.hpp"
#include "sys/print.hpp"



namespace hls {

PageTable *kernel_page_table = nullptr;

void print_table(PageTable *t) {
  kprintln("Pagetable address {}.", t);

  for (size_t i = 0; i < 512; ++i) {
    auto &entry = t->get_entry(i);

    if (entry.is_valid()) {
      kprintln(
          "Entry {}. Is leaf?  {}, Pointed address: {}. Permissions: {}{}{}", i,
          entry.is_leaf(), entry.as_pointer(),
          entry.is_executable() ? 'x' : '-', entry.is_writable() ? 'w' : '-',
          entry.is_readable() ? 'r' : '-');

      if (!entry.is_leaf()) {
        print_table(entry.as_table_pointer());
      }
    }
  }
}

void *get_kernel_begin_address() { return &_text_start; }

void *get_kernel_end_address() { return &_heap_start; }

void enable_address_translation(const PageTable* table) {
  asm volatile(
        "add a0, x0, %0;"
        "srli a0, a0, 12;"
        "li a1, 0x9000000000000000;"
        "or a0, a0, a1;"
        "csrrw a0, satp, a0;"
        :
        : "r"(table));
}

const PageTable* disable_address_transaltion() {
  const PageTable* table;
  
  asm volatile(
      "add a0, x0, x0;"
      "csrrw a0, satp, a0;"
      "add %0, x0, a0;"
      "slli %0, %0, 12;"
      : "=r"(table)
      :
      );

  return table;
}

const PageTable* get_current_page_table() {
  const PageTable* table;

  asm volatile(
    "add a0, x0, x0;"
    "csrrw a0, satp, a0;"
    "add a1, a0, x0;"
    "csrrw a0, satp, a0;"
    "add %0, a1, x0;"
    "slli %0, %0, 12;"
    : "=r"(table)
    :
  );

  return table;
}

}; // namespace hls