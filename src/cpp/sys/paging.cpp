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

#include "sys/paging.hpp"
#include "misc/new.hpp"
#include "sys/mem.hpp"
#include "sys/print.hpp"

namespace hls {

void setup_paging() {
  size_t heap_size = reinterpret_cast<uint64_t>(&_heap_size);

  kdebug(&_heap_start);
  kdebug(heap_size);

  kprintln("Initializing PageFrameManager with {} kb of memory.\r\n",
           heap_size / 1024);

  PageFrameManager::init(&_heap_start, heap_size);
  PageFrameManager &manager = PageFrameManager::instance();

  kprintln("PageFrameManager initialized with a total of {} page frames.",
           manager.frame_count());
}

bool PageFrameManager::is_frame_aligned(void *ptr) {
  uintptr_t p = to_uintptr_t(ptr);
  return !(p & 0xFFF);
}

Result<size_t> PageFrameManager::find_free_frame() {
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

void PageFrameManager::set_bit(size_t idx, bool value) {
  size_t bmap_idx = idx / 64;
  size_t bit_idx = idx % 64;

  m_bitmap[bmap_idx].set_bit(bit_idx, value);
}

PageFrameManager &PageFrameManager::__internal_instance(void *base_address,
                                                        size_t mem_size) {
  static PageFrameManager p = PageFrameManager(base_address, mem_size);
  return p;
}

PageFrameManager::PageFrameManager(void *base_address, size_t mem_size) {
  m_bitmap = reinterpret_cast<BMap *>(base_address);
  PageFrame<VPN::KB_VPN> *temp_frames =
      reinterpret_cast<PageFrame<VPN::KB_VPN> *>(
          align(base_address, alignof(PageFrame<VPN::KB_VPN>)));
  PageFrame<VPN::KB_VPN> *mem_end = reinterpret_cast<PageFrame<VPN::KB_VPN> *>(
      (char *)base_address + mem_size);

  if (!is_aligned(mem_end, alignof(PageFrame<VPN::KB_VPN>))) {
    mem_end = reinterpret_cast<PageFrame<VPN::KB_VPN> *>(
                  align(mem_end, alignof(PageFrame<VPN::KB_VPN>))) -
              1;
  }

  size_t temp_frame_count = mem_end - temp_frames;
  m_bitmap_count = temp_frame_count / 64 + (temp_frame_count % 64 ? 1 : 0);

  kdebug(temp_frames);
  kdebug(temp_frame_count);
  kdebug(mem_end);
  kdebug(m_bitmap_count);

  for (size_t i = 0; i < m_bitmap_count; ++i) {
    new (m_bitmap + i) BMap();
  }

  frames = reinterpret_cast<PageFrame<VPN::KB_VPN> *>(
      align(m_bitmap + m_bitmap_count, alignof(PageFrame<VPN::KB_VPN>)));

  m_frame_count = mem_end - frames;

  kdebug(frames);
  kdebug(m_bitmap + m_bitmap_count);
  kdebug(alignof(PageFrame<VPN::KB_VPN>));
}

PageFrameManager &PageFrameManager::instance() {
  bool initialized = init(nullptr, 0);
  if (initialized)
    return __internal_instance(nullptr, 0);

  // TODO: PANIC, for now return nullptr

  PageFrameManager *p = nullptr;
  return *p;
}

bool PageFrameManager::init(void *base_address, size_t mem_size) {
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

void PageFrameManager::release_frame(PageFrame<VPN::KB_VPN> *frame) {
  if (frame < frames)
    return;

  size_t idx = frame - frames;
  set_bit(idx, false);
}

Result<PageFrame<VPN::KB_VPN> *> PageFrameManager::get_frame() {
  auto result = find_free_frame();
  if (result.is_error())
    return error<PageFrame<VPN::KB_VPN> *>(result.get_error());

  size_t idx = result.get_value();

  set_bit(idx, true);

  return value(frames + idx);
}

size_t PageFrameManager::frame_count() const { return m_frame_count; }

} // namespace hls
