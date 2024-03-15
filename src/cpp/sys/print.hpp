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

extern "C" putchar_func_ptr putchar;

namespace hls {

void setup_printing();

void strprint(const char *);
void strprintln(const char *);
void ptrprint(const void *);
void intprint(int64_t);
void uintprint(uint64_t);
void floatprint(double);

template <typename T, typename... Args>
void print_v(const char *str, T p, Args... args) {
  if (str == nullptr) {
    strprint("Can't print nullptr string.");
  }

  while (*str) {
    auto c = *str;
    if (c == '{') {
      if (*(str + 1) != '}') {
        strprint("{");
        print_v(str + 1, p, args...);
        break;
      }
      using type = decltype(p);
      if constexpr (is_same_v<type, const char *>) {
        strprint(p);
      } else if constexpr (is_same_v<remove_cvref_t<type>, char>) {
        putchar(p);
      } else if constexpr (is_pointer_v<type>) {
        ptrprint(p);
      } else if constexpr (is_integral_v<type>) {
        if (is_signed<type>::value) {
          intprint(p);
        } else if constexpr (!is_signed<type>::value) {
          uintprint(p);
        }
      } else if constexpr (is_floating_point_v<type>) {
        floatprint(p);
      } else {
        strprint("Type printing not supported!");
      }
      if constexpr (sizeof...(args)) {
        print_v(str + 2, args...);
      } else {
        if (*(str + 2))
          strprint(str + 2);
      }
      break;
    }

    putchar(c);
    ++str;
  }
}

template <typename... Args> void kprint(const char *str, Args... args) {
  if constexpr (sizeof...(args) == 0) {
    strprint(str);
  } else {
    print_v(str, args...);
  }
}

template <typename... Args> void kprintln(const char *str, Args... args) {
  if constexpr (sizeof...(args) == 0) {
    strprint(str);
  } else {
    print_v(str, args...);
  }

  strprint("\r\n");
}

#define kdebug(expr)                                                           \
  if constexpr (true) {                                                        \
    if constexpr (is_integral_v<decltype(expr)>) {                             \
      void *p = reinterpret_cast<void *>(expr);                                \
      auto p2 = expr;                                                          \
      kprintln(#expr " : {} " #expr " hex: {}", p2, p);                        \
    } else {                                                                   \
      kprintln(#expr ": {}", expr);                                            \
    }                                                                          \
  }

} // namespace hls

#endif