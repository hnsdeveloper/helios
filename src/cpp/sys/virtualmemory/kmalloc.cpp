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
#include "include/arch/riscv/plat_def.h"
#include "sys/virtualmemory/common.hpp"
#include "sys/virtualmemory/memmap.hpp"
#include "sys/virtualmemory/paging.hpp"

namespace hls {

const uint64_t FRAME_SIZE = hls::PageFrame<PageLevel::KB_VPN>::s_size;

const byte FREE = 0b00;
const byte ALLOCATED = 0b01;
const byte ALLOCATED_CONTIGUOUS = 0b10;
const byte CONTIGUOUS_PART = 0b11;

const size_t MEMINFO_SIZE =
    4 * REGISTER_SIZE + 1; // 4 * 8 bytes + 1 byte for "accounting"

class MemBlockManager {
  byte *m_accounting_bits = nullptr;
  MemBlockManager *m_next = nullptr;
  void *m_mem = nullptr;
  size_t m_allocatable;

  bool in_range(void *p) {
    char *a = reinterpret_cast<char *>(p);
    char *b = reinterpret_cast<char *>(m_mem);
    char *c = reinterpret_cast<char *>(this) + FRAME_SIZE;

    return a >= b && a < c;
  }

  byte get_index(size_t i) {
    byte &b = m_accounting_bits[i / 4];
    return ((b >> ((i % 4) * 2))) & 0b11;
  };

  void set_index(size_t i, byte data) {
    byte &b = m_accounting_bits[i / 4];
    b = b | ((0b11) << ((i % 4) * 2));
    b = b ^ ((0b11) << ((i % 4) * 2));
    b = b | (((data & 0b11)) << ((i % 4) * 2));
  };

public:
  MemBlockManager() {
    size_t accounting_bits_count =
        (FRAME_SIZE - sizeof(MemBlockManager)) / MEMINFO_SIZE;
    m_allocatable = accounting_bits_count * 4;
    m_accounting_bits = reinterpret_cast<byte *>(this + 1);
    m_mem = align_forward(m_accounting_bits + accounting_bits_count, 8);
  }

  ~MemBlockManager() = default;

  void *allocate(const size_t size) {

    const size_t needed_slots = ((size / 8)) + (size % 8 ? 1 : 0);
    // We can skip checking for contiguous blocks if we know we don't
    // have enough
    if (free_blocks() >= needed_slots && size <= max_allocatable_size()) {
      size_t contiguous_available = 0;
      size_t idx_available = 0;

      size_t i = 0;
      do {
        byte state = get_index(i);
        if (state != FREE) {
          idx_available = 0;
          contiguous_available = 0;
          continue;
        }

        idx_available = contiguous_available ? idx_available : i;
        ++contiguous_available;

      } while (++i < max_allocatable_blocks() &&
               contiguous_available < needed_slots);

      if (contiguous_available == needed_slots) {
        for (size_t j = 0; j < needed_slots; ++j) {
          if (j == 0)
            set_index(idx_available, ALLOCATED);
          else
            set_index(j + idx_available, CONTIGUOUS_PART);
          --m_allocatable;
        }
        return reinterpret_cast<uint64_t *>(m_mem) + idx_available;
      }
    }

    // If we got here, it means we don't have enough (contiguous) blocks
    if (m_next) {
      return m_next->allocate(size);
    }

    return nullptr;
  }

  MemBlockManager *free(void *mem, MemBlockManager *before) {
    if (in_range(mem)) {
      size_t idx = reinterpret_cast<uint64_t *>(mem) -
                   reinterpret_cast<uint64_t *>(m_mem);

      set_index(
          idx,
          CONTIGUOUS_PART); // Lets handle the first (edge) case like this :p

      do {
        byte state = get_index(idx);
        if (state == CONTIGUOUS_PART) {
          set_index(idx, FREE);
        } else {
          break;
        }
        ++idx;
        ++m_allocatable;
      } while (idx < max_allocatable_blocks());

      if (max_allocatable_blocks() == free_blocks()) {
        if (before) {
          before->m_next = m_next;
        }
        return this;
      }
    } else if (m_next) {
      return m_next->free(mem, this);
    }

    return nullptr;
  }

  void link_next(MemBlockManager *m) {
    if (m_next)
      m_next->link_next(m);
    else
      m_next = m;
  }

  MemBlockManager *get_next() { return m_next; }

  size_t max_allocatable_size() {
    return max_allocatable_blocks() * REGISTER_SIZE;
  }

  size_t max_allocatable_blocks() {
    return ((FRAME_SIZE - sizeof(MemBlockManager)) / MEMINFO_SIZE) * 4;
  }

  size_t free_blocks() { return m_allocatable; }
} __attribute__((aligned(8)));

class KMemoryAllocator {
  MemBlockManager *m_manager_head;

public:
  KMemoryAllocator() {
    auto p_frame = PageFrameManager::instance().get_frame();
    if (p_frame.is_error()) {
      PANIC("Impossible to initialize KMemoryAllocator. Failed to get a page "
            "frame.");
    }
    kmmap(kernel_page_table, p_frame.get_value(), PageLevel::KB_VPN,
          p_frame.get_value(), true, false);
    m_manager_head = reinterpret_cast<MemBlockManager *>(p_frame.get_value());
    new (m_manager_head) MemBlockManager();
  }

  static KMemoryAllocator &instance() {
    static KMemoryAllocator allocator;
    return allocator;
  }

  void *allocate(size_t n) {
    if (n > m_manager_head->max_allocatable_size())
      return nullptr;

    void *p = nullptr;
    p = m_manager_head->allocate(n);

    if (p == nullptr) {
      auto p_frame = PageFrameManager::instance().get_frame();
      if (p_frame.is_error()) {
        // TODO: HANDLE IF ALL PAGES ARE USED. THIS SHOULD ONLY FAIL ON
        // EXCEPTIONAL CIRCUMSTANCES.
        PANIC("Failed to get page frame for KMemoryAllocator.");
      }
      kmmap(kernel_page_table, p_frame.get_value(), PageLevel::KB_VPN,
            p_frame.get_value(), true, false);
      auto frame = reinterpret_cast<MemBlockManager *>(p_frame.get_value());
      new (frame) MemBlockManager();
      m_manager_head->link_next(frame);
      p = m_manager_head->allocate(n);
    }

    return p;
  }

  void free(void *ptr) {
    if (ptr == nullptr)
      return;

    auto p = m_manager_head->free(ptr, nullptr);

    if (p == nullptr)
      return;

    // The first frame is always available.
    if (p != m_manager_head) {
      PageFrameManager::instance().release_frame(
          reinterpret_cast<hls::PageKB *>(p));
    }
  }
};

void *kmalloc(size_t n) { return KMemoryAllocator::instance().allocate(n); };

void kfree(void *ptr) { return KMemoryAllocator::instance().free(ptr); }

} // namespace hls