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

void memcpy(void *dest, const void *src, size_t bytes) {
  const char *src_c = reinterpret_cast<const char *>(src);
  char *dest_c = reinterpret_cast<char *>(dest);

  while (bytes--) {
    *dest_c = *src_c;
    ++src_c;
    ++dest_c;
  }
}

void *memmove(void *dest, const void *src, size_t bytes) {
  const char *src_c = reinterpret_cast<const char *>(src);
  char *dest_c = reinterpret_cast<char *>(dest);
  size_t i = 0;

  char *buffer = nullptr;

  asm("add %0, x0, sp" : "=r"(buffer) : "r"(buffer));

  buffer = buffer - bytes;

  for (; i < bytes; ++i) {
    buffer[i] = src_c[i];
  }

  for (; i < bytes; ++i) {
    dest_c[i] = buffer[i];
  }
  return dest;
}

int memcmp(const void *ptr1, const void *ptr2, size_t num) {
  const byte *p1 = reinterpret_cast<const byte *>(ptr1);
  const byte *p2 = reinterpret_cast<const byte *>(ptr2);

  for (size_t i = 0; i < num; ++i) {
    byte a = *(p1 + i);
    byte b = *(p2 + i);

    if (a < b)
      return -1;

    if (a > b)
      return 1;
  }

  return 0;
}

void memset(void *dest, byte c, size_t n) {
  byte *b = reinterpret_cast<byte *>(dest);

  for (size_t i = 0; i < n; ++i) {
    b[i] = c;
  }
}

const void *memchr(const void *ptr, byte ch, size_t count) {

  if (ptr != nullptr) {
    const char *p = reinterpret_cast<const char *>(ptr);
    for (size_t i = 0; i < count; ++i) {
      if (*(p + i) == ch)
        return (p + i);
    }
  }

  return nullptr;
}

uintptr_t to_uintptr_t(const void *ptr) {
  static const byte *null = nullptr;
  const byte *p = reinterpret_cast<const byte *>(ptr);
  return p - null;
}

void *to_ptr(uintptr_t v) { return (byte *)nullptr + v; }

const void *apply_offset(const void *ptr, size_t offset) {
  auto p = to_uintptr_t(ptr);
  return to_ptr(p + offset);
}

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

uint16_t byteswap16(uint16_t v) { return (v >> 8 | v << 8); }

uint32_t byteswap32(uint32_t v) {
  return ((v & 0xFF) << 24) | ((v & 0xFF00) << 8) | ((v >> 8) & 0xFF00) |
         ((v >> 24) | 0xFF);
};

uint64_t byteswap64(uint64_t v) {
  // TODO: IMPLEMENT
  return v;
}

uint16_t uint16_read_be(uint16_t data) {
  if constexpr (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) {
    return byteswap16(data);
  }
  return data;
}

uint32_t uint32_read_be(uint32_t data) {
  if constexpr (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) {
    return byteswap32(data);
  }
  return data;
}

uint64_t uint64_read_be(uint64_t data) {
  if constexpr (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) {
    return byteswap64(data);
  }
  return data;
}

} // namespace hls