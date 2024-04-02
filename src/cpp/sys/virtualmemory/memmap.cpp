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

#include "sys/virtualmemory/memmap.hpp"
#include "include/symbols.h"
#include "sys/mem.hpp"
#include "sys/print.hpp"
#include "sys/virtualmemory/paging.hpp"

hls::PageTable *kernel_page_table;

namespace hls {

uintptr_t calculate_virtual_ptr_offset(void *vaddress) {
  uintptr_t p = to_uintptr_t(vaddress);
  return p & 0xFFF;
}

size_t get_page_entry_index(PageLevel v, const void *vaddress) {
  size_t vpn_idx = static_cast<size_t>(v);
  uintptr_t idx = to_uintptr_t(vaddress) >> 12;
  return (idx >> (vpn_idx * 9)) & 0x1FF;
}

Result<PageLevel> walk_table(PageTable **table_ptr, const void *vaddress,
                             PageLevel page_level) {
  PageTable *table = *table_ptr;

  if (table == nullptr) {
    return error<PageLevel>(Error::INVALID_PAGE_TABLE);
  }

  // We can't walk the table anymore, thus everything stays the same
  if (page_level == PageLevel::KB_VPN) {
    return error<PageLevel>(Error::VALUE_LIMIT_REACHED);
  }

  uintptr_t vpn = get_page_entry_index(page_level, vaddress);
  auto &entry = table->get_entry(vpn);

  if (!entry.is_valid()) {
    return error<PageLevel>(Error::INVALID_PAGE_ENTRY);
  }

  if (!entry.is_leaf()) {
    *table_ptr = entry.as_table_pointer();
    page_level = next_vpn(page_level);
  }

  return value(page_level);
}

Result<void *> get_physical_address(PageTable *start_table,
                                    const void *vaddress) {
  if (start_table == nullptr)
    return error<void *>(Error::INVALID_PAGE_TABLE);

  for (size_t i = 0; i < static_cast<size_t>(PageLevel::LAST_VPN) + 1; ++i) {
    PageLevel v =
        static_cast<PageLevel>(static_cast<size_t>(PageLevel::LAST_VPN) - i);
    size_t idx = get_page_entry_index(v, vaddress);
    auto &entry = start_table->get_entry(idx);

    if (!entry.is_valid())
      break;

    if (!entry.is_leaf()) {
      start_table = entry.as_table_pointer();
      continue;
    }

    auto p = to_uintptr_t(entry.as_pointer());
    p |= (to_uintptr_t(vaddress) & 0xFFF);

    for (size_t j = 0; j < static_cast<size_t>(v); ++j) {
      size_t offset = get_page_entry_index(static_cast<PageLevel>(j), vaddress);
      p |= offset << (12 + 9 * j);
    }

    return value(to_ptr(p));
  }

  return error<void *>(Error::INVALID_PAGE_ENTRY);
}

// TODO: IMPLEMENT FLAGS FOR MAPED ADDRESSES
Result<const void *> kmmap(PageTable *start_table, const void *vaddress,
                           PageLevel page_level, const void *physical_address,
                           bool writable, bool executable) {

  // First lets check if it is already mapped
  auto address_result = get_physical_address(start_table, vaddress);

  if (address_result.is_value()) {
    return error<const void *>(
        Error::ADDRESS_ALREADY_MAPPED); // If the result is an error, then that
                                        // address is not mapped
  }

  // Here we presume we are always using and starting from the highest available
  // paging mode. TODO: change algorithm to accept different starting levels!
  PageTable *table = start_table;
  PageLevel current_page_level = PageLevel::LAST_VPN;

  PageFrameManager &frame_manager = PageFrameManager::instance();

  while (current_page_level != page_level) {
    auto result = walk_table(&table, vaddress, current_page_level);
    if (result.is_error()) {
      switch (result.get_error()) {
      case Error::INVALID_PAGE_ENTRY: {
        size_t entry_idx = get_page_entry_index(current_page_level, vaddress);
        PageEntry &entry = table->get_entry(entry_idx);
        auto frame_result = frame_manager.get_frame();

        if (frame_result.is_error()) {
          return error<const void *>(frame_result.get_error());
        }

        PageTable *new_table =
            reinterpret_cast<PageTable *>(frame_result.get_value());

        entry.point_to_table(new_table);
        table = new_table;
        current_page_level = next_vpn(current_page_level);
        continue;
      }
      case Error::INVALID_PAGE_TABLE:
        return error<const void *>(result.get_error());
      default: {
        PANIC("Unhandled error while walking page tables.");
      }
      }
    }
    current_page_level = result.get_value();
  }

  auto &entry =
      table->get_entry(get_page_entry_index(current_page_level, vaddress));

  auto point_lambda = [](PageEntry &entry, const void *addr,
                         PageLevel page_level) {
    switch (page_level) {
    case PageLevel::TB_VPN:
      entry.point_to_frame<PageLevel::TB_VPN>(
          reinterpret_cast<const PageFrame<PageLevel::TB_VPN> *>(addr));
      break;
    case PageLevel::GB_VPN:
      entry.point_to_frame<PageLevel::GB_VPN>(
          reinterpret_cast<const PageFrame<PageLevel::GB_VPN> *>(addr));
      break;
    case PageLevel::MB_VPN:
      entry.point_to_frame<PageLevel::MB_VPN>(
          reinterpret_cast<const PageFrame<PageLevel::MB_VPN> *>(addr));
      break;
    case PageLevel::KB_VPN:
      entry.point_to_frame<PageLevel::KB_VPN>(
          reinterpret_cast<const PageFrame<PageLevel::KB_VPN> *>(addr));
      break;
    }
  };

  point_lambda(entry, physical_address, page_level);

  if (executable) {
    entry.make_executable(true);
  }
  if (writable) {
    entry.make_writable(true);
  }

  return value(vaddress);
}

const void *get_kernel_begin_address() { return &_text_start; }

const void *get_kernel_end_address() { return &_heap_start; }

bool map_kernel(PageTable *table) {
  for (const char *p = (const char *)get_kernel_begin_address();
       p < get_kernel_end_address(); p += PageKB::s_size) {
    auto result = kmmap(kernel_page_table, p, PageLevel::KB_VPN, p, true, true);
    if (result.is_error())
      return false;
  }

  return true;
}

bool is_address_mapped(PageTable *table, void *vaddress) {
  return !get_physical_address(table, vaddress).is_error();
}

void *setup_kernel_memory_mapping(void *fdt) {

  PageFrameManager &manager = PageFrameManager::instance();
  kernel_page_table =
      reinterpret_cast<PageTable *>(manager.get_frame().get_value());

  memset(kernel_page_table, 0, sizeof(PageTable));
  map_kernel(kernel_page_table);

  return kernel_page_table;
}

} // namespace hls