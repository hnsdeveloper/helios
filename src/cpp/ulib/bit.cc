export module Bit;

import Expected;
import Memory;

#include "types.h"

extern "C" size_t _popcount(size_t data);

namespace hls {

template <size_t N> struct CalculateSize {

  static constexpr size_t value =
      N < sizeof(byte) * 8
          ? 1
          : N / (sizeof(byte) * 8) + (N % (sizeof(byte) * 8) ? 1 : 0);
};

export template <size_t N> class Bit {
  static constexpr size_t s_data_size = CalculateSize<N>::value;

  byte data[s_data_size];

  size_t calculate_buffer_index(size_t n) const {
    size_t result = (N - n) / 8;

    return result;
  }

public:
  Bit() { memset(data, 0, s_data_size); }

  ~Bit() = default;

  size_t popcount() const {
    size_t accumulator = 0;

    // TODO: IMPLEMENT MORE EFFICIENT VERSION
    for (size_t i = 0; i < s_data_size; ++i) {
      accumulator += _popcount(data[i]);
    }

    return accumulator;
  }

  Expected<bool> get_bit(size_t n) const {
    if (n >= s_data_size * 8) {
      return error<bool>(Error::OUT_OF_BOUNDS);
    }

    size_t i = calculate_buffer_index(n);
    size_t j = n % 8;
    auto &b = data[i];

    bool result = (b >> j) & 0x1;

    return value(result);
  }

  void set_bit(size_t n, bool val) {
    if (n >= s_data_size * 8)
      return;

    size_t i = calculate_buffer_index(n);
    size_t j = n % 8;

    byte &b = data[i];

    byte temp = 0x1u & val;

    b = b | (temp << j);
  }

  size_t size() const { return N; }

  void flip() {
    for (size_t i = 0; i < s_data_size; ++i) {
      data[i] = ~data[i];
    }
  }
};

} // namespace hls