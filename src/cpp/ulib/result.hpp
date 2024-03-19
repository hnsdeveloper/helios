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

#ifndef _EXPECTED_HPP_
#define _EXPECTED_HPP_

#include "include/types.h"
#include "include/typetraits.h"
#include "include/utilities.h"
#include "misc/new.hpp"
#include "sys/panic.hpp"

namespace hls {

enum class Error : uint64_t {
  UNDEFINED_ERROR,
  INVALID_INDEX,
  INVALID_PAGE_TABLE,
  INVALID_PAGE_ENTRY,
  PAGE_ALREADY_MAPPED,
  ADDRESS_ALREADY_MAPPED,
  OUT_OF_MEMORY,
  OUT_OF_BOUNDS,
  VALUE_LIMIT_REACHED,
  READ_NOT_ALLOWED,
  WRITE_NOT_ALLOWED,
  EXECUTE_NOT_ALLOWED,
  OPERATION_NOT_ALLOWED
};

template <typename T>
  requires(!is_reference_v<T>)
class Result {

  union {
    Error e;
    T value;
  } m_stored;

  bool m_is_error = false;

  Result() {}

  Result(const T &v) { new (&m_stored.value) T(v); }

  Result(T &&v) {
    using type = remove_cvref_t<T>;
    new (&m_stored.value) type(hls::move(v));
  }

  Result(Error e) {
    m_stored.e = e;
    m_is_error = true;
  }

public:
  Result(Result &&other) {
    if (other.is_error()) {
      m_stored.e = other.m_stored.e;
    } else {
      auto *p = &m_stored.value;
      new (p) T((hls::move(other.m_stored.value)));
    }
  }

  Result(const Result &other) {
    if (other.is_error()) {
      m_stored.e = other.m_stored.e;
    } else {
      auto *p = &m_stored.value;
      new (p) T((other.m_stored.value));
    }
  }

  ~Result() {
    if (is_value()) {
      m_stored.value.~T();
    }
  }

  using value_type = T;
  using error_type = Error;

  bool is_error() const { return m_is_error; }

  bool is_value() const { return !is_error(); }

  value_type &get_value() {
    const auto &a = *this;
    return const_cast<value_type &>(a.get_value());
  }

  // Given that a value is expected, in Kernel code if it doesn't hold, we
  // panic, after all, the system might end in a unusable state if we consume
  // the value as if it was proper.
  const value_type &get_value() const {
    if (is_error()) {
      PANIC("Result contains error instead of expected value.");
    }
    return m_stored.value;
  }

  Error get_error() const {
    if (!is_error()) {
      PANIC("Result contains value instead of error.");
    }
    return m_stored.e;
  }

  static auto error(Error e) {
    using type = remove_cvref_t<T>;
    return Result<type>(e);
  }

  static auto value(T &&v) {
    using type = remove_cvref_t<T>;

    return Result<type>(hls::move(v));
  }

  static Result value(const T &v) {
    using type = remove_cvref_t<T>;
    return Result<type>(v);
  }
};

template <typename T> auto error = &Result<T>::error;

auto value(auto v) {
  using type = remove_cvref_t<decltype(v)>;
  return Result<type>::value(hls::forward<decltype(v)>(v));
}

} // namespace hls

#endif