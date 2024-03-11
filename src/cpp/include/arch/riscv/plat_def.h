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

#include "include/types.h"

namespace hls {

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

static const size_t KB_VPN = 0;
static const size_t MB_VPN = 1;
static const size_t GB_VPN = 2;
static const size_t TB_VPN = 3;

enum class VPN : size_t { KB_VPN = 0, MB_VPN = 1, GB_VPN = 2, TB_VPN = 3 };

struct PageEntry;
struct PageTable;
struct PageFrame;

struct __attribute__((packed)) __attribute__((aligned(PAGE_TABLE_ENTRY_SIZE)))
PageEntry {
  uint64_t data = 0;

  void make_writable(bool v);
  void make_readable(bool v);
  void make_executable(bool v);

  void make_leaf();
  bool is_valid();

  bool is_leaf();

  bool is_writable();
  bool is_readable();
  bool is_executable();

  void point_to_table(PageTable *table);
  void point_to_frame(PageFrame *frame);
};

struct __attribute__((packed)) __attribute__((aligned(4096))) PageTable {
  __attribute__((packed)) PageEntry entries[ENTRIES_PER_TABLE];

  PageEntry &get_entry(size_t entry_index);
};

struct __attribute__((packed)) __attribute__((aligned(4096))) PageFrame {
  char data[PAGE_FRAME_SIZE];

  PageTable *as_table();
};

size_t get_vpn_index(void *v_address, VPN vpn);

} // namespace hls

#endif