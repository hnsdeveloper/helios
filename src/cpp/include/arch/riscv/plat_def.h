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
#include "ulib/expected.hpp"

namespace hls {

Result<uint64_t> read_csr(MCSR csr_number);
Result<uint64_t> write_csr(MCSR csr_code, uint64_t data);

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

template <VPN n> struct PAGESIZE;

template <> struct PAGESIZE<VPN::KB_VPN> {
  static constexpr size_t size = 4096;
  static constexpr VPN page_type = VPN::KB_VPN;
};

template <> struct PAGESIZE<VPN::MB_VPN> {
  static constexpr size_t size = 512ul * PAGESIZE<VPN::KB_VPN>::size;
  static constexpr VPN page_type = VPN::MB_VPN;
};

template <> struct PAGESIZE<VPN::GB_VPN> {
  static constexpr size_t size = 512ul * PAGESIZE<VPN::MB_VPN>::size;
  static constexpr VPN page_type = VPN::GB_VPN;
};

template <> struct PAGESIZE<VPN::TB_VPN> {
  static constexpr size_t size = 512ul * PAGESIZE<VPN::GB_VPN>::size;
  static constexpr VPN page_type = VPN::TB_VPN;
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

  void point_to_table(PageTable *table);

  template <VPN v> void point_to_frame(PageFrame<v> *frame) {
    static constexpr uintptr_t HIGH_BITS = 0xff10000000000000;

    using frame_t = PageFrame<v>;
    if (is_aligned(frame, alignof(frame_t))) {
      data = to_uintptr_t(frame) >> 12;
      data = ((data << 9 | 0x1ul) | HIGH_BITS) ^ HIGH_BITS;
      this->make_readable(true);
    }
  }

  PageTable *as_table_pointer();

  template <VPN v> PageFrame<v> *as_frame_pointer() {
    return reinterpret_cast<PageFrame<v> *>(((data >> 9) << 12));
  };
};

struct __attribute__((packed)) PageTable {
  PageEntry entries[ENTRIES_PER_TABLE];

  PageEntry &get_entry(size_t entry_index);
};

template <VPN v> struct __attribute__((packed)) PageFrame {
  char data[PAGESIZE<v>::size];

  PageTable *as_table() {
    if (PAGESIZE<v>::page_type == VPN::KB_VPN)
      return reinterpret_cast<PageTable *>(this);

    return nullptr;
  }
};

size_t get_vpn_index(void *v_address, VPN vpn);

} // namespace hls

#endif