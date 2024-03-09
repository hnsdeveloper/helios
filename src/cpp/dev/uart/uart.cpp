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

#include "dev/uart/uart.hpp"
#include "dev/mmio.hpp"
#include "include/types.h"
#include "sys/mem.hpp"

namespace hls {
// TODO: IMPLEMENTED FOR SPECIFIC DEVICE. MAKE GENERIC IMPLEMENTATION THAT FINDS
// UART AND SETS IT UP ACCORDING TO MODEL

volatile void *get_uart_base_address() {
  uintptr_t uint_p = 0x10000000;
  volatile void *p = reinterpret_cast<volatile void *>(to_ptr(uint_p));
  return p;
}

void uart_init(volatile void *uart_base_address) {
  const size_t LCR_OFFSET = 3;
  const size_t FIFO_OFFSET = 2;
  const size_t BUFFER_INTERRUPT_OFFSET = 1;

  // Line control register
  byte lcr = 0x1 << 0 | 0x1 << 1;
  byte fifo = 0x1 << 0;
  byte b_int = 0x1 << 0;

  mmio_write(uart_base_address, LCR_OFFSET, lcr);
  mmio_write(uart_base_address, FIFO_OFFSET, fifo);
  mmio_write(uart_base_address, BUFFER_INTERRUPT_OFFSET, b_int);

  // Divisor baud rate... for qemu it doesn't matter
  const uint16_t DIVISOR = 592;
  uint8_t divisor_least = DIVISOR & 0xff;
  uint8_t divisor_most = DIVISOR >> 8;

  mmio_write(uart_base_address, LCR_OFFSET, lcr | (1 << 7));

  mmio_write(uart_base_address, 0, divisor_least);
  mmio_write(uart_base_address, 1, divisor_most);

  mmio_write(uart_base_address, LCR_OFFSET, lcr);
}

void uart_putchar(char c) {
  volatile void *addr = get_uart_base_address();
  mmio_write(addr, 0, reinterpret_cast<uint8_t &>(c));
}

void uprint(char c) { uart_putchar(c); }

putchar_func_ptr setup_uart_as_print() {
  volatile void *uart_address = get_uart_base_address();
  uart_init(uart_address);
  return &uprint;
}

} // namespace hls