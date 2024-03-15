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

#ifndef _PAGING_HPP_
#define _PAGING_HPP_

#include "include/arch/riscv/plat_def.h"
#include "include/global.h"
#include "include/symbols.h"
#include "ulib/bit.hpp"
#include "ulib/expected.hpp"

const size_t BITSET_SIZE = 64;

namespace hls {

using BMap = Bit<BITSET_SIZE>;

class PageFrameManager {
  PageKB *m_frames = nullptr;
  BMap *m_bitmap;

  size_t m_frame_count;
  size_t m_bitmap_count;

  bool is_frame_aligned(void *ptr);
  Result<size_t> find_free_frame();
  void set_bit(size_t idx, bool value);

  static PageFrameManager &__internal_instance(void *base_address,
                                               size_t mem_size);

  PageFrameManager(void *base_address, size_t mem_size);

public:
  PageFrameManager(const PageFrameManager &) = delete;
  PageFrameManager(PageFrameManager &&) = delete;
  ~PageFrameManager() {}

  size_t frame_count() const;
  Result<PageKB *> get_frame();
  void release_frame(PageKB *frame);

  static PageFrameManager &instance();
  static bool init(void *base_address, size_t mem_size);
};

void setup_paging();

} // namespace hls

#endif