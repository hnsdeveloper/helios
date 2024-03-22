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

uintptr_t calculate_virtual_ptr_offset(void *vaddress) {
  uintptr_t p = to_uintptr_t(vaddress);
  return p & 0xFFF;
}

size_t get_page_entry_index(VPN v, void *vaddress) {
  size_t vpn_idx = static_cast<size_t>(v);
  uintptr_t idx = to_uintptr_t(vaddress) >> 12;
  return (idx >> (vpn_idx * 9)) & 0x1FF;
}

Result<VPN> walk_table(PageTable **table_ptr, void *vaddress, VPN page_level) {
  PageTable *table = *table_ptr;

  if (table == nullptr) {
    return error<VPN>(Error::INVALID_PAGE_TABLE);
  }

  // We can't walk the table anymore, thus everything stays the same
  if (page_level == VPN::KB_VPN) {
    return error<VPN>(Error::VALUE_LIMIT_REACHED);
  }

  uintptr_t vpn = get_page_entry_index(page_level, vaddress);
  auto &entry = table->get_entry(vpn);

  if (!entry.is_valid()) {
    return error<VPN>(Error::INVALID_PAGE_ENTRY);
  }

  if (!entry.is_leaf()) {
    *table_ptr = entry.as_table_pointer();
    page_level = next_vpn(page_level);
  }

  return value(page_level);
}

Result<void *> get_physical_address(PageTable *start_table, void *vaddress) {
  if (start_table == nullptr)
    return error<void *>(Error::INVALID_PAGE_TABLE);

  for (size_t i = 0; i < static_cast<size_t>(VPN::LAST_VPN) + 1; ++i) {
    VPN v = static_cast<VPN>(static_cast<size_t>(VPN::LAST_VPN) - i);
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
      size_t offset = get_page_entry_index(static_cast<VPN>(j), vaddress);
      p |= offset << (12 + 9 * j);
    }

    return value(to_ptr(p));
  }

  return error<void *>(Error::INVALID_PAGE_ENTRY);
}

bool is_address_used(void *address) { return false; }

// TODO: IMPLEMENT MAPPING "REAL" VIRTUAL ADDRESSES

// TODO: IMPLEMENT FLAGS FOR MAPED ADDRESSES
Result<void *> kmmap(PageTable *start_table, void *vaddress, VPN page_level,
                     void *physical_address, bool writable = false,
                     bool executable = false) {

  // First lets check if it is already mapped
  auto address_result = get_physical_address(start_table, vaddress);

  if (address_result.is_value()) {
    return error<void *>(
        Error::ADDRESS_ALREADY_MAPPED); // If the result is an error, then that
                                        // address is not mapped
  }

  // Here we presume we are always using and starting from the highest available
  // paging mode. TODO: change algorithm to accept different starting levels!
  PageTable *table = start_table;
  VPN current_page_level = VPN::LAST_VPN;

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
          return error<void *>(frame_result.get_error());
        }

        PageTable *new_table =
            reinterpret_cast<PageTable *>(frame_result.get_value());

        entry.point_to_table(new_table);
        table = new_table;
        current_page_level = next_vpn(current_page_level);
        continue;
      }
      case Error::INVALID_PAGE_TABLE:
        return error<void *>(result.get_error());
      default: {
        PANIC("Unhandled error while walking page tables.");
      }
      }
    }
    current_page_level = result.get_value();
  }

  auto &entry =
      table->get_entry(get_page_entry_index(current_page_level, vaddress));

  auto point_lambda = [](PageEntry &entry, void *addr, VPN page_level) {
    switch (page_level) {
    case VPN::TB_VPN:
      entry.point_to_frame<VPN::TB_VPN>(
          reinterpret_cast<PageFrame<VPN::TB_VPN> *>(addr));
      break;
    case VPN::GB_VPN:
      entry.point_to_frame<VPN::GB_VPN>(
          reinterpret_cast<PageFrame<VPN::GB_VPN> *>(addr));
      break;
    case VPN::MB_VPN:
      entry.point_to_frame<VPN::MB_VPN>(
          reinterpret_cast<PageFrame<VPN::MB_VPN> *>(addr));
      break;
    case VPN::KB_VPN:
      entry.point_to_frame<VPN::KB_VPN>(
          reinterpret_cast<PageFrame<VPN::KB_VPN> *>(addr));
      break;
    }
  };

  bool map_to_physical = true;

  if (map_to_physical) {
    point_lambda(entry, physical_address, page_level);
  } else {
    // Find frame, create it and blablabla
    // TODO: IMPLEMENT
  }

  if (executable) {
    entry.make_executable(true);
  }
  if (writable) {
    entry.make_writable(true);
  }

  return value(vaddress);
}

void print_table(PageTable *t) {
  kprintln("Pagetable address {}.", t);

  for (size_t i = 0; i < 512; ++i) {
    auto &entry = t->get_entry(i);

    if (entry.is_valid()) {
      kprintln(
          "Entry {}. Is leaf?  {}, Pointed address: {}. Permissions: {}{}{}", i,
          entry.is_leaf(), entry.as_pointer(),
          entry.is_executable() ? 'w' : '-', entry.is_writable() ? 'w' : '-',
          entry.is_readable() ? 'r' : '-');

      if (!entry.is_leaf()) {
        print_table(entry.as_table_pointer());
      }
    }
  }
}

void *setup_kernel_memory_mapping() {
  // TODO: For now the only "machine" supported is the QEMU virtual machine,
  // thus addresses
  //  are hardcoded. With time we shall refactor the code and get the addresses
  //  dynamically
  PageFrameManager &manager = PageFrameManager::instance();

  kernel_page_table =
      reinterpret_cast<PageTable *>(manager.get_frame().get_value());
  memset(kernel_page_table, 0, sizeof(PageTable));
  kmmap(kernel_page_table, to_ptr(0x00000000), VPN::GB_VPN, to_ptr(0x00000000),
        true, true);
  kmmap(kernel_page_table, to_ptr(0x40000000), VPN::GB_VPN, to_ptr(0x40000000),
        true, true);

  for (byte *c = &_text_start; c < &_stack_end; c += 4096) {
    kmmap(kernel_page_table, c, VPN::KB_VPN, c, true, true);
  };

  // Now we have the kernel mapped!!!
  // write_csr(hls::MCSR::mstatus, to_uintptr_t(kernel_page_table));

  return kernel_page_table;
}

} // namespace hls