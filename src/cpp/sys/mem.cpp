/*---------------------------------------------------------------------------------
MIT License

Copyright (c) 2024 Helio Nunes Santos

                Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including without
limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom
the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
                copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
                AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

---------------------------------------------------------------------------------*/
#include "include/types.h"

namespace hls {

void memcpy(void *dest, void *src, size_t bytes) {
  char *src_c = reinterpret_cast<char *>(src);
  char *dest_c = reinterpret_cast<char *>(dest);

  while (bytes--) {
    *dest_c = *src_c;
    ++src_c;
    ++dest_c;
  }
}

void memset(void *dest, byte c, size_t n) {
  byte *b = reinterpret_cast<byte *>(dest);

  for (size_t i = 0; i < n; ++i) {
    b[i] = c;
  }
}

uintptr_t to_uintptr_t(const void *ptr) {
  static const byte *null = nullptr;
  const byte *p = reinterpret_cast<const byte *>(ptr);
  return p - null;
}

void *to_ptr(uintptr_t v) { return (byte *)nullptr + v; }

bool is_aligned(const void *ptr, size_t alignment) {
  uintptr_t p = to_uintptr_t(ptr);
  return (p / alignment * alignment) == p;
}

void *align_forward(const void *ptr, size_t alignment) {
  uintptr_t p = to_uintptr_t(ptr);

  if (alignment == 1 || is_aligned(ptr, alignment))
    return to_ptr(p);

  // Checks if aligment is valid. Must be multiple of a power of 2.
  if (!((alignment - 1) & alignment)) {
    p = (p / alignment) * alignment + alignment;
    return to_ptr(p);
  }

  return nullptr;
}

void *align_back(const void *ptr, size_t alignment) {
  uintptr_t p = to_uintptr_t(ptr);
  if (alignment == 1 || is_aligned(ptr, alignment))
    return to_ptr(p);

  p = (p / alignment) * alignment;
  return to_ptr(p);
}

} // namespace hls