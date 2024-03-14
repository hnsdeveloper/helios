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

using namespace hls;

Expected<uint64_t> hls::read_csr(MCSR csr_number) {
  uint64_t result;
  using enum MCSR;

#define __GENERATE_READ_CASE(event)                                            \
  case event:                                                                  \
    asm("csrr %0, " #event : "=r"(result));                                    \
    break;
  switch (csr_number) {
    __GENERATE_READ_CASE(mvendorid)
    __GENERATE_READ_CASE(marchid)
    __GENERATE_READ_CASE(mimpid)
    __GENERATE_READ_CASE(mhartid)
    __GENERATE_READ_CASE(mstatus)
    __GENERATE_READ_CASE(misa)
    __GENERATE_READ_CASE(medeleg)
    __GENERATE_READ_CASE(mideleg)
    __GENERATE_READ_CASE(mie)
    __GENERATE_READ_CASE(mtvec)
    __GENERATE_READ_CASE(mcounteren)
    __GENERATE_READ_CASE(mscratch)
    __GENERATE_READ_CASE(mepc)
    __GENERATE_READ_CASE(mcause)
    __GENERATE_READ_CASE(mtval)
    __GENERATE_READ_CASE(mip)
    __GENERATE_READ_CASE(pmpcfg0)
    __GENERATE_READ_CASE(pmpcfg1)
    __GENERATE_READ_CASE(pmpcfg2)
    __GENERATE_READ_CASE(pmpcfg3)
    __GENERATE_READ_CASE(pmpaddr0)
    __GENERATE_READ_CASE(pmpaddr1)
    __GENERATE_READ_CASE(pmpaddr2)
    __GENERATE_READ_CASE(pmpaddr3)
    __GENERATE_READ_CASE(pmpaddr4)
    __GENERATE_READ_CASE(pmpaddr5)
    __GENERATE_READ_CASE(pmpaddr6)
    __GENERATE_READ_CASE(pmpaddr7)
    __GENERATE_READ_CASE(pmpaddr8)
    __GENERATE_READ_CASE(pmpaddr9)
    __GENERATE_READ_CASE(pmpaddr10)
    __GENERATE_READ_CASE(pmpaddr11)
    __GENERATE_READ_CASE(pmpaddr12)
    __GENERATE_READ_CASE(pmpaddr13)
    __GENERATE_READ_CASE(pmpaddr14)
    __GENERATE_READ_CASE(pmpaddr15)
    __GENERATE_READ_CASE(mcycle)
    __GENERATE_READ_CASE(minstret)
    __GENERATE_READ_CASE(mhpmcounter3)
    __GENERATE_READ_CASE(mhpmcounter4)
    __GENERATE_READ_CASE(mhpmcounter5)
    __GENERATE_READ_CASE(mhpmcounter6)
    __GENERATE_READ_CASE(mhpmcounter7)
    __GENERATE_READ_CASE(mhpmcounter8)
    __GENERATE_READ_CASE(mhpmcounter9)
    __GENERATE_READ_CASE(mhpmcounter10)
    __GENERATE_READ_CASE(mhpmcounter11)
    __GENERATE_READ_CASE(mhpmcounter12)
    __GENERATE_READ_CASE(mhpmcounter13)
    __GENERATE_READ_CASE(mhpmcounter14)
    __GENERATE_READ_CASE(mhpmcounter15)
    __GENERATE_READ_CASE(mhpmcounter16)
    __GENERATE_READ_CASE(mhpmcounter17)
    __GENERATE_READ_CASE(mhpmcounter18)
    __GENERATE_READ_CASE(mhpmcounter19)
    __GENERATE_READ_CASE(mhpmcounter20)
    __GENERATE_READ_CASE(mhpmcounter21)
    __GENERATE_READ_CASE(mhpmcounter22)
    __GENERATE_READ_CASE(mhpmcounter23)
    __GENERATE_READ_CASE(mhpmcounter24)
    __GENERATE_READ_CASE(mhpmcounter25)
    __GENERATE_READ_CASE(mhpmcounter26)
    __GENERATE_READ_CASE(mhpmcounter27)
    __GENERATE_READ_CASE(mhpmcounter28)
    __GENERATE_READ_CASE(mhpmcounter29)
    __GENERATE_READ_CASE(mhpmcounter30)
    __GENERATE_READ_CASE(mhpmcounter31)
    __GENERATE_READ_CASE(mhpmevent3)
    __GENERATE_READ_CASE(mhpmevent4)
    __GENERATE_READ_CASE(mhpmevent5)
    __GENERATE_READ_CASE(mhpmevent6)
    __GENERATE_READ_CASE(mhpmevent7)
    __GENERATE_READ_CASE(mhpmevent8)
    __GENERATE_READ_CASE(mhpmevent9)
    __GENERATE_READ_CASE(mhpmevent10)
    __GENERATE_READ_CASE(mhpmevent11)
    __GENERATE_READ_CASE(mhpmevent12)
    __GENERATE_READ_CASE(mhpmevent13)
    __GENERATE_READ_CASE(mhpmevent14)
    __GENERATE_READ_CASE(mhpmevent15)
    __GENERATE_READ_CASE(mhpmevent16)
    __GENERATE_READ_CASE(mhpmevent17)
    __GENERATE_READ_CASE(mhpmevent18)
    __GENERATE_READ_CASE(mhpmevent19)
    __GENERATE_READ_CASE(mhpmevent20)
    __GENERATE_READ_CASE(mhpmevent21)
    __GENERATE_READ_CASE(mhpmevent22)
    __GENERATE_READ_CASE(mhpmevent23)
    __GENERATE_READ_CASE(mhpmevent24)
    __GENERATE_READ_CASE(mhpmevent25)
    __GENERATE_READ_CASE(mhpmevent26)
    __GENERATE_READ_CASE(mhpmevent27)
    __GENERATE_READ_CASE(mhpmevent28)
    __GENERATE_READ_CASE(mhpmevent29)
    __GENERATE_READ_CASE(mhpmevent30)
    __GENERATE_READ_CASE(mhpmevent31)
    __GENERATE_READ_CASE(mcontext)
#undef __GENERATE_READ_CASE
  default:
    return error<uint64_t>(Error::INVALID_INDEX);
  }

  return value(result);
}

Expected<uint64_t> hls::write_csr(MCSR csr_number, uint64_t data) {
  using enum MCSR;
  switch (csr_number) {
#define __GENERATE_WRITE_CASE(enum_name)                                       \
  case enum_name:                                                              \
    asm("csrw " #enum_name ", %0" : : "r"(data));                              \
    break;
  case mvendorid:
    [[falltrhough]];
  case marchid:
    [[falltrhough]];
  case mimpid:
    [[falltrhough]];
  case mhartid:
    return error<uint64_t>(Error::WRITE_NOT_ALLOWED);
    __GENERATE_WRITE_CASE(mstatus)
    __GENERATE_WRITE_CASE(misa)
    __GENERATE_WRITE_CASE(medeleg)
    __GENERATE_WRITE_CASE(mideleg)
    __GENERATE_WRITE_CASE(mie)
    __GENERATE_WRITE_CASE(mtvec)
    __GENERATE_WRITE_CASE(mcounteren)
    __GENERATE_WRITE_CASE(mscratch)
    __GENERATE_WRITE_CASE(mepc)
    __GENERATE_WRITE_CASE(mcause)
    __GENERATE_WRITE_CASE(mtval)
    __GENERATE_WRITE_CASE(mip)
    __GENERATE_WRITE_CASE(pmpcfg0)
    __GENERATE_WRITE_CASE(pmpcfg1)
    __GENERATE_WRITE_CASE(pmpcfg2)
    __GENERATE_WRITE_CASE(pmpcfg3)
    __GENERATE_WRITE_CASE(pmpaddr0)
    __GENERATE_WRITE_CASE(pmpaddr1)
    __GENERATE_WRITE_CASE(pmpaddr2)
    __GENERATE_WRITE_CASE(pmpaddr3)
    __GENERATE_WRITE_CASE(pmpaddr4)
    __GENERATE_WRITE_CASE(pmpaddr5)
    __GENERATE_WRITE_CASE(pmpaddr6)
    __GENERATE_WRITE_CASE(pmpaddr7)
    __GENERATE_WRITE_CASE(pmpaddr8)
    __GENERATE_WRITE_CASE(pmpaddr9)
    __GENERATE_WRITE_CASE(pmpaddr10)
    __GENERATE_WRITE_CASE(pmpaddr11)
    __GENERATE_WRITE_CASE(pmpaddr12)
    __GENERATE_WRITE_CASE(pmpaddr13)
    __GENERATE_WRITE_CASE(pmpaddr14)
    __GENERATE_WRITE_CASE(pmpaddr15)
    __GENERATE_WRITE_CASE(mcycle)
    __GENERATE_WRITE_CASE(minstret)
    __GENERATE_WRITE_CASE(mhpmcounter3)
    __GENERATE_WRITE_CASE(mhpmcounter4)
    __GENERATE_WRITE_CASE(mhpmcounter5)
    __GENERATE_WRITE_CASE(mhpmcounter6)
    __GENERATE_WRITE_CASE(mhpmcounter7)
    __GENERATE_WRITE_CASE(mhpmcounter8)
    __GENERATE_WRITE_CASE(mhpmcounter9)
    __GENERATE_WRITE_CASE(mhpmcounter10)
    __GENERATE_WRITE_CASE(mhpmcounter11)
    __GENERATE_WRITE_CASE(mhpmcounter12)
    __GENERATE_WRITE_CASE(mhpmcounter13)
    __GENERATE_WRITE_CASE(mhpmcounter14)
    __GENERATE_WRITE_CASE(mhpmcounter15)
    __GENERATE_WRITE_CASE(mhpmcounter16)
    __GENERATE_WRITE_CASE(mhpmcounter17)
    __GENERATE_WRITE_CASE(mhpmcounter18)
    __GENERATE_WRITE_CASE(mhpmcounter19)
    __GENERATE_WRITE_CASE(mhpmcounter20)
    __GENERATE_WRITE_CASE(mhpmcounter21)
    __GENERATE_WRITE_CASE(mhpmcounter22)
    __GENERATE_WRITE_CASE(mhpmcounter23)
    __GENERATE_WRITE_CASE(mhpmcounter24)
    __GENERATE_WRITE_CASE(mhpmcounter25)
    __GENERATE_WRITE_CASE(mhpmcounter26)
    __GENERATE_WRITE_CASE(mhpmcounter27)
    __GENERATE_WRITE_CASE(mhpmcounter28)
    __GENERATE_WRITE_CASE(mhpmcounter29)
    __GENERATE_WRITE_CASE(mhpmcounter30)
    __GENERATE_WRITE_CASE(mhpmcounter31)
    __GENERATE_WRITE_CASE(mhpmevent3)
    __GENERATE_WRITE_CASE(mhpmevent4)
    __GENERATE_WRITE_CASE(mhpmevent5)
    __GENERATE_WRITE_CASE(mhpmevent6)
    __GENERATE_WRITE_CASE(mhpmevent7)
    __GENERATE_WRITE_CASE(mhpmevent8)
    __GENERATE_WRITE_CASE(mhpmevent9)
    __GENERATE_WRITE_CASE(mhpmevent10)
    __GENERATE_WRITE_CASE(mhpmevent11)
    __GENERATE_WRITE_CASE(mhpmevent12)
    __GENERATE_WRITE_CASE(mhpmevent13)
    __GENERATE_WRITE_CASE(mhpmevent14)
    __GENERATE_WRITE_CASE(mhpmevent15)
    __GENERATE_WRITE_CASE(mhpmevent16)
    __GENERATE_WRITE_CASE(mhpmevent17)
    __GENERATE_WRITE_CASE(mhpmevent18)
    __GENERATE_WRITE_CASE(mhpmevent19)
    __GENERATE_WRITE_CASE(mhpmevent20)
    __GENERATE_WRITE_CASE(mhpmevent21)
    __GENERATE_WRITE_CASE(mhpmevent22)
    __GENERATE_WRITE_CASE(mhpmevent23)
    __GENERATE_WRITE_CASE(mhpmevent24)
    __GENERATE_WRITE_CASE(mhpmevent25)
    __GENERATE_WRITE_CASE(mhpmevent26)
    __GENERATE_WRITE_CASE(mhpmevent27)
    __GENERATE_WRITE_CASE(mhpmevent28)
    __GENERATE_WRITE_CASE(mhpmevent29)
    __GENERATE_WRITE_CASE(mhpmevent30)
    __GENERATE_WRITE_CASE(mhpmevent31)
    __GENERATE_WRITE_CASE(mcontext)
#undef __GENERATE_WRITE_CASE
  default:
    return error<uint64_t>(Error::INVALID_INDEX);
  }

  return read_csr(csr_number);
}

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

bool PageEntry::is_valid() {
  bool result = data & 0x1;

  if (is_writable() && !is_readable())
    result = false;

  return result;
}

bool PageEntry::is_leaf() {
  return is_valid() && (is_readable() || is_executable());
}

bool PageEntry::is_writable() { return data & (1u << WRITE_BIT); }
bool PageEntry::is_readable() { return data & (1u << READ_BIT); }
bool PageEntry::is_executable() { return data & (1u << EXECUTE_BIT); }

PageTable *PageEntry::as_table_pointer() {
  return reinterpret_cast<PageTable *>(to_ptr(data));
}

void PageEntry::point_to_table(PageTable *table) {
  if (is_aligned(table, alignof(PageTable))) {
    data = to_uintptr_t(table) + 0x1ul;
  }
}

PageEntry &PageTable::get_entry(size_t entry_index) {
  return entries[entry_index];
}

size_t get_page_entry_index(void *v_address, VPN vpn) {
  size_t result = 0;
  size_t vpn_idx = static_cast<size_t>(vpn);
  auto addr = to_uintptr_t(v_address);
  result = (addr >> (12 + (vpn_idx * 9))) & 0x1FF;

  return result;
}
