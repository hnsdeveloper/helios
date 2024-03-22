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

#ifndef _PLAT_DEF_H_
#define _PLAT_DEF_H_

#include "include/arch/riscv/registers.h"
#include "include/types.h"
#include "sys/mem.hpp"
#include "ulib/result.hpp"

namespace hls {

Result<uint64_t> read_csr(MCSR csr_address);
Result<uint64_t> write_csr(MCSR csr_address, uint64_t data);

static const size_t PAGE_LEVELS = 4;
static const size_t PAGE_FRAME_SIZE = 4096;
static const size_t PAGE_FRAME_ALIGNMENT = PAGE_FRAME_SIZE;
static const size_t PAGE_TABLE_SIZE = PAGE_FRAME_SIZE;
static const size_t PAGE_TABLE_ENTRY_SIZE = 8;
static const size_t ENTRIES_PER_TABLE = PAGE_FRAME_SIZE / PAGE_TABLE_ENTRY_SIZE;

static const size_t VALID_PAGE_BIT = 0;
static const size_t READ_BIT = 1;
static const size_t WRITE_BIT = 2;
static const size_t EXECUTE_BIT = 3;

enum class VPN : size_t {
  KB_VPN = 0,
  MB_VPN = 1,
  GB_VPN = 2,
  TB_VPN = 3,
  LAST_VPN = TB_VPN
};

VPN next_vpn(VPN v);

template <VPN...> struct FrameInfo;

template <> struct FrameInfo<VPN::KB_VPN> {
  static constexpr size_t size = 4096;
  static constexpr VPN page_type = VPN::KB_VPN;
  static constexpr size_t alignment = size;
};

template <> struct FrameInfo<VPN::MB_VPN> {
  static constexpr size_t size = 512ul * FrameInfo<VPN::KB_VPN>::size;
  static constexpr VPN page_type = VPN::MB_VPN;
  static constexpr size_t alignment = size;
};

template <> struct FrameInfo<VPN::GB_VPN> {
  static constexpr size_t size = 512ul * FrameInfo<VPN::MB_VPN>::size;
  static constexpr VPN page_type = VPN::GB_VPN;
  static constexpr size_t alignment = size;
};

template <> struct FrameInfo<VPN::TB_VPN> {
  static constexpr size_t size = 512ul * FrameInfo<VPN::GB_VPN>::size;
  static constexpr VPN page_type = VPN::TB_VPN;
  static constexpr size_t alignment = size;
};

template <> struct FrameInfo<> {
  static constexpr size_t size_query(VPN v) {
    switch (v) {
    case VPN::KB_VPN:
      return FrameInfo<VPN::KB_VPN>::size;
    case VPN::MB_VPN:
      return FrameInfo<VPN::MB_VPN>::size;
    case VPN::GB_VPN:
      return FrameInfo<VPN::GB_VPN>::size;
    case VPN::TB_VPN:
      return FrameInfo<VPN::TB_VPN>::size;
    default: {
      PANIC("Invalid page level.");
    }
    }
  };

  static constexpr size_t alignment_query(VPN v) {
    switch (v) {
    case VPN::KB_VPN:
      return FrameInfo<VPN::KB_VPN>::alignment;
    case VPN::MB_VPN:
      return FrameInfo<VPN::MB_VPN>::alignment;
    case VPN::GB_VPN:
      return FrameInfo<VPN::GB_VPN>::alignment;
    case VPN::TB_VPN:
      return FrameInfo<VPN::TB_VPN>::alignment;
    default: {
      PANIC("Invalid page level.");
    }
    }
  }
};

struct PageEntry;
struct PageTable;
template <VPN type> struct PageFrame;

struct __attribute__((packed)) PageEntry {
  uint64_t data = 0;

  void make_writable(bool v);
  void make_readable(bool v);
  void make_executable(bool v);

  bool is_valid();

  bool is_leaf();
  bool is_table_pointer();

  bool is_writable();
  bool is_readable();
  bool is_executable();

  void *as_pointer();

  PageTable *as_table_pointer();

  void point_to_table(PageTable *table);

  template <VPN v> void point_to_frame(PageFrame<v> *frame) {
    using frame_t = PageFrame<v>;
    if (is_aligned(frame, alignof(frame_t))) {
      data = to_uintptr_t(frame) >> 2;
      data |= 0x1;
      make_readable(true);
    }
  }

  template <VPN v> PageFrame<v> *as_frame_pointer() {
    return reinterpret_cast<PageFrame<v> *>(as_pointer());
  };
};

struct __attribute__((packed)) PageTable {
  PageEntry entries[ENTRIES_PER_TABLE];

  PageEntry &get_entry(size_t entry_index);

  void print_entries();
};

template <VPN v> struct __attribute__((packed)) PageFrame {
  static constexpr size_t s_size = FrameInfo<v>::size;
  static constexpr VPN s_type = FrameInfo<v>::page_type;
  static constexpr size_t alignment = FrameInfo<v>::size;

  char data[FrameInfo<v>::size];

  PageTable *as_table() {
    if constexpr (FrameInfo<v>::page_type == VPN::KB_VPN)
      return reinterpret_cast<PageTable *>(this);

    return nullptr;
  }
};

using PageKB = PageFrame<VPN::KB_VPN>;
using PageMB = PageFrame<VPN::MB_VPN>;
using PageGB = PageFrame<VPN::GB_VPN>;
using PageTB = PageFrame<VPN::TB_VPN>;

size_t get_vpn_index(void *v_address, VPN vpn);
} // namespace hls

#endif