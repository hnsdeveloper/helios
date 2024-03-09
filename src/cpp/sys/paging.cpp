module Paging;

import Memory;
import Expected;
import Bit;
import Print;

#include "arch/riscv/plat_def.h"
#include "new.h"
#include "symbols.h"

namespace hls {

const size_t BITSET_SIZE = 64;
using BMap = Bit<BITSET_SIZE>;

class PageFrameManager {
  PageFrame *frames = nullptr; // A pointer to the first frame
  BMap *m_bitmap;
  size_t m_frame_count;

  bool is_frame_aligned(void *ptr) {
    uintptr_t p = to_uintptr_t(ptr);
    return !(p & 0xFFF);
  }

  size_t get_frame_index(PageFrame *frame) { return frame - frames; }

  static PageFrameManager &__internal_instance(void *base_address,
                                               size_t mem_size) {
    static PageFrameManager p = PageFrameManager(base_address, mem_size);
    return p;
  }

  PageFrameManager(void *base_address, size_t mem_size) {
    m_bitmap = reinterpret_cast<BMap *>(align(base_address, alignof(BMap)));
    m_frame_count = mem_size / PAGE_FRAME_SIZE;

    size_t bitmap_count = m_frame_count / 64 + m_frame_count % 64;

    for (size_t i = 0; i < bitmap_count; ++i) {
      new (m_bitmap + i) BMap();
    }

    frames = reinterpret_cast<PageFrame *>(
        align(m_bitmap + bitmap_count, alignof(PageFrame)));
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

  void free_frame(PageFrame *frame) {
    if (frame < frames)
      return;

    if (!is_frame_aligned(frame))
      return;

    size_t bmap_idx = (frame - frames) / BITSET_SIZE;

    BMap &b = m_bitmap[bmap_idx];

    b.set_bit((frame - frames) % BITSET_SIZE, 0x0);
  }

  Expected<PageFrame *> get_frame() {
    size_t ceiling = 0;

    for (size_t i = 0; i < ceiling; ++i) {
      auto &bmap = m_bitmap[i];
      if (bmap.popcount() == bmap.size())
        continue;

      for (size_t j = 0; j < bmap.size(); ++j) {
        bool b = bmap.get_bit(j).get_value();
        if (b == false) {
          bmap.set_bit(j, true);
          return value(frames + i * BITSET_SIZE + j);
        }
      }
    }

    return error<PageFrame *>(Error::OUT_OF_MEMORY);
  }
};

void setup_paging() {
  void *START_ADDRESS = &_heap_start;
  size_t MEM_SIZE = 1024 * 1024 * 1024;
  PageFrameManager::init(START_ADDRESS, MEM_SIZE);

  PageFrameManager &manager = PageFrameManager::instance();
  auto result = manager.get_frame();
}

} // namespace hls
