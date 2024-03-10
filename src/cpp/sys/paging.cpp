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

#include "include/arch/riscv/plat_def.h"
#include "include/symbols.h"
#include "misc/new.hpp"
#include "sys/mem.hpp"
#include "sys/print.hpp"
#include "ulib/bit.hpp"
#include "ulib/expected.hpp"

namespace hls {

const size_t BITSET_SIZE = 64;
using BMap = Bit<BITSET_SIZE>;

class PageFrameManager {
  PageFrame *frames = nullptr; // A pointer to the first frame
  BMap *m_bitmap;
  size_t m_frame_count;
  size_t m_bitmap_count;

  bool is_frame_aligned(void *ptr) {
    uintptr_t p = to_uintptr_t(ptr);
    return !(p & 0xFFF);
  }

  Expected<size_t> find_free_frame() {
    for (size_t i = 0; i < m_bitmap_count; ++i) {
      auto &bmap = m_bitmap[i];

      if (bmap.popcount() == bmap.size())
        continue;

      for (size_t j = 0; j < BMap::size(); ++j) {
        bool b = bmap.get_bit(j).get_value();
        size_t idx = i * BMap::size() + j;

        if (b != true && idx < m_frame_count)
          return value(i * BMap::size() + j);
      }
    }

    return error<size_t>(Error::OUT_OF_MEMORY);
  }

  void set_bit(size_t idx, bool value) {
    size_t bmap_idx = idx / 64;
    size_t bit_idx = idx % 64;

    m_bitmap[bmap_idx].set_bit(bit_idx, value);
  }

  static PageFrameManager &__internal_instance(void *base_address,
                                               size_t mem_size) {
    static PageFrameManager p = PageFrameManager(base_address, mem_size);
    return p;
  }

  PageFrameManager(void *base_address, size_t mem_size) {
    m_bitmap = reinterpret_cast<BMap *>(base_address);
    PageFrame *temp_frames =
        reinterpret_cast<PageFrame *>(align(base_address, alignof(PageFrame)));
    PageFrame *mem_end =
        reinterpret_cast<PageFrame *>((char *)base_address + mem_size);

    if (!is_aligned(mem_end, alignof(PageFrame))) {
      mem_end =
          reinterpret_cast<PageFrame *>(align(mem_end, alignof(PageFrame))) - 1;
    }

    size_t temp_frame_count = mem_end - temp_frames;
    m_bitmap_count = temp_frame_count / 64 + (temp_frame_count % 64 ? 1 : 0);

    debug("temp_frames: ")(temp_frames)(" mem_end: ")(
        mem_end)(" temp_frame_count: ")(temp_frame_count)(" m_bitmap_count: ")(
        m_bitmap_count)("\r\n");

    for (size_t i = 0; i < m_bitmap_count; ++i) {
      new (m_bitmap + i) BMap();
    }

    frames = reinterpret_cast<PageFrame *>(
        align(m_bitmap + m_bitmap_count, alignof(PageFrame)));

    m_frame_count = mem_end - frames;

    debug(" frames: ")(frames)(" m_bitmap + bitmap_count: ")(m_bitmap +
                                                             m_bitmap_count)(
        " alignof(PageFrame)")(alignof(PageFrame))("\r\n");
  }

public:
  PageFrameManager(const PageFrameManager &) = delete;
  PageFrameManager(PageFrameManager &&) = delete;
  ~PageFrameManager() {}

  static PageFrameManager &instance() {
    bool initialized = init(nullptr, 0);
    if (initialized)
      return __internal_instance(nullptr, 0);

    // TODO: PANIC, for now return nullptr

    PageFrameManager *p = nullptr;
    return *p;
  }

  static bool init(void *base_address, size_t mem_size) {
    static bool is_initialized = false;

    if (!is_initialized) {
      if (base_address == nullptr || mem_size == 0)
        return is_initialized;
      else
        __internal_instance(base_address, mem_size);
      is_initialized = true;
    }

    return is_initialized;
  }

  void release_frame(PageFrame *frame) {
    if (frame < frames)
      return;

    size_t idx = frame - frames;
    set_bit(idx, false);
  }

  Expected<PageFrame *> get_frame() {
    auto result = find_free_frame();
    if (result.is_error())
      return error<PageFrame *>(result.get_error());

    size_t idx = result.get_value();

    set_bit(idx, true);

    return value(frames + idx);
  }

  size_t frame_count() const { return m_frame_count; }
};

void setup_paging() {
  void *START_ADDRESS = &_heap_start;
  size_t MEM_SIZE = 1024 * 1024 * 10 - (&_heap_start - &_text_start);

  debug("HEAP START ADDRESS IS: ")(START_ADDRESS)("\r\n");
  debug("HEAP END ADDRESS IS: ")((char *)(START_ADDRESS) + MEM_SIZE)("\r\n");

  print("Initializing PageFrameManager with ")(MEM_SIZE /
                                               1024)("kb of memory.\n");

  PageFrameManager::init(START_ADDRESS, MEM_SIZE);
  PageFrameManager &manager = PageFrameManager::instance();

  debug("Total of ")(manager.frame_count())(" page frames.\r\n");
  for (size_t i = 0; true; ++i) {
    auto result = manager.get_frame();
    if (result.is_error()) {
      debug("Error code: ")(result.get_error());
      break;
    }

    PageFrame *frame_1 = result.get_value();
    print("Frame address:")(frame_1)(" Frame #:")(i)("\r\n");
  }

  /*
  PageFrame *frame_1 = manager.get_frame().get_value();
  PageFrame *frame_2 = manager.get_frame().get_value();
  PageFrame *frame_3 = manager.get_frame().get_value();
  PageFrame *frame_4 = manager.get_frame().get_value();
  */

  /*
  print("Frame address:")(frame_2)("\r\n");
  print("Frame address:")(frame_3)("\r\n");
  print("Frame address:")(frame_4)("\r\n");

  manager.release_frame(frame_2);
  manager.release_frame(frame_3);

  frame_2 = nullptr;
  frame_3 = nullptr;
  frame_2 = manager.get_frame().get_value();
  frame_3 = manager.get_frame().get_value();

  print("Frame address:")(frame_1)("\r\n");
  print("Frame address:")(frame_2)("\r\n");
  print("Frame address:")(frame_3)("\r\n");
  print("Frame address:")(frame_4)("\r\n");
  */
}

} // namespace hls
