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

#ifndef _MEM_HPP_
#define _MEM_HPP_

#include "include/types.h"

#define NULL nullptr

namespace hls {

void memcpy(void *dest, const void *src, size_t bytes);
void* memmove(void* dest, const void* src, size_t bytes);
void memset(void *dest, byte c, size_t n);
int memcmp(const void *ptr1, const void *ptr2, size_t num);
const void *memchr(const void *ptr, byte ch, size_t count);
uintptr_t to_uintptr_t(const void *ptr);
void *to_ptr(uintptr_t v);

void *apply_offset(const void *ptr, size_t offset);

bool is_aligned(const void *ptr, size_t alignment);
void *align_forward(const void *ptr, size_t alignment);

uint16_t endian_uint16_read(uint16_t data);
uint32_t endian_uint32_read(uint32_t data);
uint64_t endian_uint64_read(uint64_t data);

uint16_t byteswap16(uint16_t v);
uint32_t byteswap32(uint32_t v);
uint64_t byteswap64(uint64_t v);

template <typename T> inline T byteswap(T val) {
  if constexpr (sizeof(T) == 2) {
    return byteswap16(val);
  }
  if constexpr (sizeof(T) == 4) {
    return byteswap32(val);
  }
  if constexpr (sizeof(T) == 8) {
    return byteswap64(val);
  }
  return val;
}

} // namespace hls

#endif