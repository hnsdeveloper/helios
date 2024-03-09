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

#ifndef _MMIO_HPP_
#define _MMIO_HPP_

#include "include/types.h"
#include "include/utilities.h"
#include "sys/mem.hpp"
#include "ulib/expected.hpp"

// TODO: implement
bool is_mmio_address(volatile void *address) {
  if (address)
    return true;
  return false;
}

namespace hls {

// Writes val to a MMIO address + offset (in bytes).
template <typename IntegerType>
Expected<IntegerType> mmio_write(volatile void *address, size_t offset,
                                 IntegerType val) {
  if (is_mmio_address(address)) {
    volatile IntegerType *addr = reinterpret_cast<volatile IntegerType *>(
        reinterpret_cast<volatile char *>(address) + offset);
    *addr = val;
    return Expected<IntegerType>::value(val);
  }

  return Expected<IntegerType>::error(Error{});
}

// Reads a IntegerType value from device_address + offset (in bytes).
template <typename IntegerType>
Expected<IntegerType> mmio_read(volatile void *address, size_t offset) {
  if (is_mmio_address) {
    volatile IntegerType *addr = reinterpret_cast<volatile IntegerType *>(
        reinterpret_cast<volatile char *>(address) + offset);
    return value(*addr);
  }

  return error<IntegerType>(Error{});
}

} // namespace hls

#endif