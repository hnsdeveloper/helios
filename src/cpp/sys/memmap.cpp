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

#include "sys/memmap.hpp"
#include "include/symbols.h"
#include "sys/mem.hpp"
#include "sys/paging.hpp"
#include "sys/print.hpp"

hls::PageTable *kernel_page_table;

namespace hls {

uintptr_t calculate_virtual_ptr_offset(VPN vpn, void *vaddress) {
  uintptr_t vaddress_uint = to_uintptr_t(vaddress);
  uintptr_t p = 0;
  p += vaddress_uint & 0xfff;

  for (size_t i = 0; i < static_cast<size_t>(vpn); ++i) {
    p += (vaddress_uint >> (12 + i * 9)) & 0x1FF;
  }

  return p;
}

size_t get_page_entry_index(VPN v, void *vaddress) {
  size_t vpn_idx = static_cast<size_t>(v);
  uintptr_t p = to_uintptr_t(vaddress);
  return (p >> (12 + vpn_idx * 9)) & 0x1ff;
}

Result<VPN> walk_table(PageTable **table_ptr, void *vaddress, VPN current_vpn) {

  PageTable *table = *table_ptr;

  if (table == nullptr) {
    return error<VPN>(Error::INVALID_PAGE_TABLE);
  }

  // We can't walk the table anymore, thus everything stays the same
  if (current_vpn == VPN::KB_VPN) {
    return value(current_vpn);
  }

  uintptr_t vpn = get_page_entry_index(current_vpn, vaddress);

  auto &entry = table->get_entry(vpn);

  if (!entry.is_valid()) {
    return error<VPN>(Error::INVALID_PAGE_ENTRY);
  }

  if (!entry.is_leaf()) {
    *table_ptr = entry.as_table_pointer();
  }

  size_t i = static_cast<size_t>(current_vpn);

  return value(static_cast<VPN>(i - entry.is_leaf() ? 0 : 1));
}

Result<void *> get_physical_address(PageTable *start_table, void *vaddress) {
  if (start_table == nullptr)
    return error<void *>(Error::INVALID_PAGE_TABLE);

  VPN last_vpn = VPN::TB_VPN;
  PageTable *table = start_table;
  while (true) {
    auto result = walk_table(&table, vaddress, last_vpn);
    if (result.is_error()) {
      return error<void *>(result.get_error());
    }
    VPN current_vpn = result.get_value();
    if (current_vpn == last_vpn)
      break;
    last_vpn = current_vpn;
  }

  size_t vpn_idx = get_page_entry_index(last_vpn, vaddress);
  PageEntry &entry = table->get_entry(vpn_idx);
  uintptr_t p = 0;

  if (!entry.is_valid() || !entry.is_leaf()) {
    return error<void *>(Error::INVALID_PAGE_ENTRY);
  }

  switch (last_vpn) {
  case VPN::TB_VPN:
    p = to_uintptr_t(entry.as_frame_pointer<VPN::TB_VPN>());
    break;
  case VPN::GB_VPN:
    p = to_uintptr_t(entry.as_frame_pointer<VPN::GB_VPN>());
    break;
  case VPN::MB_VPN:
    p = to_uintptr_t(entry.as_frame_pointer<VPN::MB_VPN>());
    break;
  case VPN::KB_VPN:
    p = to_uintptr_t(entry.as_frame_pointer<VPN::KB_VPN>());
    break;
  }

  p += calculate_virtual_ptr_offset(last_vpn, vaddress);

  return value(to_ptr(p));
}

bool is_address_used(void *address) { return false; }

void *kmmap(PageTable *start_table, void *vaddress, VPN page_level,
            void *physical_address) {

  // First lets check if it is already mapped
  auto g_address_result = get_physical_address(start_table, vaddress);
  if (g_address_result.is_value())
    return g_address_result.get_value();

  PageTable *table = start_table;
  VPN current_page_level = VPN::LAST_VPN;

  while (current_page_level != page_level) {
    auto result = walk_table(&start_table, vaddress, current_page_level);
    if (result.is_error()) // Theoretically nullptr is still a valid value
      return nullptr;
  }
}

void setup_kernel_memory_mapping() {
  PageFrameManager &manager = PageFrameManager::instance();
  auto result = manager.get_frame();
  if (result.is_error()) {
    // print("Can't get kernel page frame.");
  }

  kernel_page_table = reinterpret_cast<PageTable *>(result.get_value());
  memset(kernel_page_table, 0, sizeof(PageTable));

  kmmap(kernel_page_table, to_ptr(0x00000000), VPN::GB_VPN, to_ptr(0x00000000));

  kmmap(kernel_page_table, to_ptr(0x40000000), VPN::GB_VPN, to_ptr(0x40000000));
}

} // namespace hls