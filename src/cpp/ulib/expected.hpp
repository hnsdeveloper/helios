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

namespace hls {

enum class Error : uint64_t {
  UNDEFINED_ERROR,
  INVALID_INDEX,
  INVALID_PAGE_TABLE,
  OUT_OF_MEMORY,
  OUT_OF_BOUNDS
};

template <typename T>
  requires(!is_reference_v<T>)
class Expected {

  union {
    Error e;
    T value;
  } m_stored;

  bool m_is_error = false;

  Expected() {}

  Expected(const T &v) {
    using type = remove_cvref_t<T>;
    if constexpr (!is_integral_v<T> && !is_pointer_v<T> &&
                  !is_floating_point_v<T>) {
      new (&m_stored.value) type(v);
    } else {
      m_stored.value = v;
    }
  }
  Expected(T &&v) {
    using type = remove_cvref_t<T>;
    if constexpr (!is_integral_v<T> && !is_pointer_v<T> &&
                  !is_floating_point_v<T>) {
      new (&m_stored.value) type(hls::move(v));
    } else {
      m_stored.value = v;
    }
  }
  Expected(Error e) {
    m_stored.e = e;
    m_is_error = true;
  }

public:
  Expected(Expected &&other) {
    using type = T;
    if (other.is_value()) {
      auto &v = other.m_stored.value;
      if constexpr (!is_integral_v<T> && !is_pointer_v<T> &&
                    !is_floating_point_v<T>) {
        new (&m_stored.value) type(hls::move(v));
        other.m_store.value.~T();
      } else {
        m_stored.e = other.m_stored.e;
      }
    }
  }

  ~Expected() {
    if (is_value()) {
      m_stored.value.~T();
    }
  }

  using value_type = T;
  using error_type = Error;

  bool is_error() const { return m_is_error; }

  bool is_value() const { return !is_error(); }

  value_type &get_value() { return m_stored.value; }

  const value_type &get_value() const { return m_stored.value; }

  Error get_error() { return m_stored.e; }

  static auto error(Error e) {
    using type = remove_cvref_t<T>;
    return Expected<type>(e);
  }

  static auto value(T &&v) {
    using type = remove_cvref_t<T>;

    return Expected<type>(hls::move(v));
  }

  static Expected value(const T &v) {
    using type = remove_cvref_t<T>;
    return Expected<type>(v);
  }
};

template <typename T> auto error = &Expected<T>::error;

auto value(auto v) {
  using type = remove_cvref_t<decltype(v)>;
  return Expected<type>::value(hls::forward<decltype(v)>(v));
}

} // namespace hls

#endif