export module Bit;

import Expected;
import Memory;

#include "types.h"

extern "C" size_t _popcount(size_t data);

namespace hls {

template <size_t N> struct CalculateSize {

  static constexpr size_t value =
      N < sizeof(char) * 8
          ? 1
          : N / (sizeof(char) * 8) + (N % (sizeof(char) * 8) ? 1 : 0);
};

export template <size_t N> class Bit {
  static constexpr size_t s_data_size = CalculateSize<N>::value;

  char data[s_data_size];

public:
  Bit() = default;
  ~Bit() = default;

  size_t popcount() const {
    size_t bits = 0;
    size_t accumulator = 0;
    if (s_data_size > sizeof(size_t)) {
      for (size_t i = 0; i < s_data_size; ++i) {
      }
    } else {
      memcpy(&bits, data, sizeof(data));
      accumulator += _popcount(data);
    }

    return accumulator;
  }

  Expected<bool> get_bit(size_t n) const {
    if (n >= sizeof(uint64_t))
      return error<bool>(Error::OUT_OF_BOUNDS);

    bool result = false;

    return value(result);
  }

  void set_bit(size_t n, bool val) {
    if (n >= sizeof(uint64_t))
      return;
  }

  size_t bit_count() const { return N; }
};

} // namespace hls