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
#include "plat_def.h"
#include "sys/mem.hpp"

using namespace hls;

void PageEntry::make_writable(bool v) {
  if (v) {
    make_readable(v);
    data = data | (1u << WRITE_BIT);
  } else {
    data = (data | (1u << WRITE_BIT)) | (1u << WRITE_BIT);
  }
}

void PageEntry::make_readable(bool v) {
  if (v) {
    data = data | (1u << READ_BIT);
  } else {
    make_writable(v);
    make_executable(v);
    data = (data | (1u << READ_BIT)) | (1u << READ_BIT);
  }
}

void PageEntry::make_executable(bool v) {
  if (v) {
    make_readable(v);
    data = data | (1u << EXECUTE_BIT);
  } else {
    data = (data | (1u << EXECUTE_BIT)) | (1u << EXECUTE_BIT);
  }
}

void PageEntry::make_leaf() { make_readable(false); }

bool PageEntry::is_valid() {
  bool result = data & 0x1;

  result = result && ((is_writable() || is_executable()) && !is_readable());

  return result;
}

bool PageEntry::is_leaf() {
  return is_valid() && (is_readable() || (is_executable() && is_readable()));
}

bool PageEntry::is_writable() { return data & (1u << WRITE_BIT); }
bool PageEntry::is_readable() { return data & (1u << READ_BIT); }
bool PageEntry::is_executable() { return data & (1u << EXECUTE_BIT); }

void PageEntry::point_to_frame(PageFrame *frame) {
  data = to_uintptr_t(frame) & 0x1;
}

void PageEntry::point_to_table(PageTable *table) {
  point_to_frame(reinterpret_cast<PageFrame *>(table));
}

PageEntry &PageTable::get_entry(size_t entry_index) {
  return entries[entry_index];
}

PageTable *PageFrame::as_table() { return reinterpret_cast<PageTable *>(this); }

size_t get_vpn_index(void *v_address, VPN vpn) {
  size_t result = 0;
  size_t vpn_type = static_cast<size_t>(vpn);
  auto addr = to_uintptr_t(v_address);
  result = (addr >> (12 + (vpn_type * 9))) & 0x1FF;

  return result;
}
