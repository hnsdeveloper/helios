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

#include "include/arch/riscv/plat_def.h"
#include "sys/memmap.hpp"

namespace hls {

template <typename T> class VirtualPointer {
  T *m_ptr;
  const PageTable *m_table;

  const T *get_ptr() {
    auto ptr = get_physical_address(m_table, m_ptr);
    if (ptr.is_value())
      return reinterpret_cast<const T *>(ptr.get_value());

    // TODO: When filesystem is available, handle retrieving from swap if
    // possible

    PANIC("Virtual pointer pointing to invalid address.");
  }

public:
  VirtualPointer(T *ptr, const PageTable *table) : m_ptr(ptr), m_table(table){};
  VirtualPointer(const VirtualPointer &other)
      : m_ptr(other.m_ptr), m_table(other.m_table){};
  ~VirtualPointer() = default;

  T *operator->() {
    const auto &self = *this;
    return const_cast<T *>(self.operator->());
  }
  const T *operator->() const { return get_ptr(); };

  T &operator*() {
    const auto &self = *this;
    return const_cast<T &>(self.operator*());
  }

  const T &operator*() const { return *get_ptr(); }

  T *get_vaddress_ptr() { return m_ptr; }

  const T *get_vaddress_ptr() const { return m_ptr; }
};

template <typename T> class VirtualPointer<const T *>;

template <typename T> auto make_vptr(T *ptr, const PageTable *table) {
  return VirtualPointer<T>(ptr, table);
}

}; // namespace hls