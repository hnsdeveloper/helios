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

#ifndef _VIRTUALPOINTER_HPP_
#define _VIRTUALPOINTER_HPP_

#include "include/arch/riscv/plat_def.h"
#include "sys/memmap.hpp"

namespace hls {

template <typename T> class VPtr {
public:
  using type = T;

  ~VPtr(){};

  T &operator*() { return *get_physical_address(); }
  T *operator->() { return get_physical_address(); }

  T *get_vaddress() { return m_ptr; }
  T *get_physical_address() { VPN current_page_level = m_page_level; }

private:
  type *m_ptr;
  PageTable *m_table;
  VPN m_page_level;
};

// We don't have the need of this for now, but I will leave it here as to
// remember.
// template <typename T> class VPtr<T *> {
//
//  T *m_ptr;
//  PageTable *m_table;
//  VPN m_page_level;
//
// public:
//  ~VPtr(){};
//  V
//};

template <typename T>
VPtr make_vptr(T *vaddr, PageTable *table, VPN page_level) {}

} // namespace hls

#endif