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

#include "sys/virtualmemory/kmalloc.hpp"
#include "sys/virtualmemory/common.hpp"
#include "sys/virtualmemory/memmap.hpp"

namespace hls {

class KernelMemoryAllocator {

  static KernelMemoryAllocator &__internal_instance() {
    static KernelMemoryAllocator allocator;
    return allocator;
  }

  KernelMemoryAllocator(){

  };

public:
  static bool init() { return false; }

  static KernelMemoryAllocator &instance() {}

  void *allocate(size_t n) { return nullptr; }
  void free(const void *ptr) {
    (void)0;
    return;
  }
};

void *kmalloc(size_t n) {
  return KernelMemoryAllocator::instance().allocate(n);
};

void kfree(void *ptr) { return KernelMemoryAllocator::instance().free(ptr); }

void initialize_kmalloc() {}
} // namespace hls