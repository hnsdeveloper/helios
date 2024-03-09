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
#ifndef _PRINT_HPP_
#define _PRINT_HPP_

#include "include/types.h"
#include "include/typetraits.h"

namespace hls {

void setup_printing();

void strprint(const char *);
void strprintln(const char *);
void ptrprint(const void *);
void intprint(int64_t);
void uintprint(uint64_t);
void floatprint(double);

auto print(auto p) {
  using type = decltype(p);

  if constexpr (is_same_v<type, const char *>) {
    strprint(p);
  } else if constexpr (is_pointer_v<type>) {
    ptrprint(p);
  } else if constexpr (is_integral_v<type>) {
    if (signed_integral_v<type>) {
      intprint(p);
    } else if constexpr (!signed_integral_v<type>) {
      uintprint(p);
    }
  } else if constexpr (is_floating_point_v<type>) {
    floatprint(p);
  }

  auto rec_lamb = [](auto z) { return print(z); };
  return rec_lamb;
}

auto debug(auto p) {
  constexpr bool debug_enabled =
      true; // For now we will have it here, but at some point kernel debugging
            // should become optional
  if (debug_enabled)
    return print(p);
}

} // namespace hls

#endif