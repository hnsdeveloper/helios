/*---------------------------------------------------------------------------------
MIT License

Copyright (c) 2022 Helio Nunes Santos

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
module Print;

import UART;
import Memory;

#include "types.h"

const bool UART_PRINTING = true;

void empty_putchar(char _c) { _c = _c; }

putchar_func_ptr putchar = &empty_putchar;

namespace hls {

void setup_printing() {
  if (UART_PRINTING) {
    putchar = setup_uart_as_print();
  }
}

void strprint(const char *str) {
  while (*str) {
    ::putchar(*str);
    ++str;
  }
}

void strprintln(const char *str) {
  strprint(str);
  strprint("\r\n");
}

void ptrprint(void *ptr) {
  auto v = to_uintptr_t(ptr);
  constexpr size_t buffer_size = sizeof(v) * 8 / 4;
  char buffer[buffer_size];

  for (size_t i = 0; i < buffer_size; ++i) {
    char c = v & 0xF;

    if (c <= 9) {
      c += '0';
    } else {
      c += 'A' - 10;
    }

    buffer[i] = c;

    v = v >> 4;
  }

  for (size_t i = 0; i < buffer_size; ++i) {
    char &c = buffer[buffer_size - i - 1];
    putchar(c);
  }
}

void ptrprintln(void *ptr) {
  ptrprint(ptr);
  strprint("\r\n");
}

void intprint(int64_t v) {
  if (v >= 0)
    uintprint(0 + v);
  else {
    strprint("-");
    // Check if v is equal to INT_MIN
    if (v == ~decltype(v){0}) {
      v += 1;
      v = -v;
      uintprint(0u + v + 1);
    } else {
      v = -v;
      uintprint(0u + v);
    }
  }
}

void intprintln(int64_t v) {
  intprint(v);
  strprint("\r\n");
}

void uintprint(uint64_t v) {
  if (v == 0) {
    putchar('0');
    return;
  }

  char buffer[256];
  size_t buffer_used = 0;

  for (size_t i = 0; v; ++i, v /= 10, ++buffer_used) {
    buffer[i] = v % 10 + '0';
  }

  for (size_t i = 0; i < buffer_used; ++i) {
    char &c = buffer[buffer_used - i - 1];
    putchar(c);
  }
}

void uintprintln(uint64_t v) {
  uintprint(v);
  strprint("\r\n");
}

void floatprint(double) {
  strprint("Floating point values printing not supported yet!");
}

} // namespace hls
