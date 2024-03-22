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

#include "sys/string.hpp"

namespace hls {

int strcmp(const char *str1, const char *str2) {
  while (true) {
    auto a = *reinterpret_cast<const unsigned char *>(str1);
    auto b = *reinterpret_cast<const unsigned char *>(str2);
    // If we reach the end of both strings, they are equal
    if (a == 0 && b == 0) {
      return 0;
    } else if (a < b) {
      return -1;
    } else if (a > b) {
      return 1;
    }

    ++str1;
    ++str2;
  }
}

int strncmp(const char *str1, const char *str2, size_t n) {
  for (size_t i = 0; i < n; ++i) {
    auto a = *reinterpret_cast<const unsigned char *>(str1 + i);
    auto b = *reinterpret_cast<const unsigned char *>(str2 + i);

    if (a == 0 && b == 0) {
      break;
    } else if (a < b) {
      return -1;
    } else if (a > b) {
      return 1;
    }
  }

  return 0;
}

size_t strlen(const char *str) {
  size_t i = 0;
  for (; str[i] != '\0'; ++i)
    ;
  return i;
}

const char *strchr(const char *str, char c) {
  while (true) {
    char v = *str;
    if (v == '\0')
      return nullptr;
    if (v == c)
      break;
    ++str;
  }

  return str;
}

char to_upper(char c) {
  if (c >= 'a' && c <= 'z')
    return c - 'a' + 'A';

  return c;
}

char to_lower(char c) {
  if (c >= 'A' && c <= 'Z')
    return c - 'A' + 'a';

  return c;
}

bool is_dec_digit(char c) { return c >= '0' && c <= '9'; }
bool is_letter(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
};
bool is_alphanumeric(char c) { return is_dec_digit(c) || is_letter(c); }

bool is_hex_digit(char c) {
  c = to_lower(c);
  return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
}

Result<uint64_t> hex_to_uint(const char *str) {
  if (str == nullptr)
    return error<uint64_t>(Error::INVALID_ARGUMENT);

  if (*str != '0' && to_lower(*(str + 1)) != 'x') {
    return error<uint64_t>(Error::INVALID_ARGUMENT);
  }

  uint64_t temp = 0;
  for (auto str2 = str + 2; *str2; ++str2) {
    char c = to_lower(*str2);
    if (!is_hex_digit(c)) {
      return error<uint64_t>(Error::INVALID_ARGUMENT);
    }

    c = c <= '9' ? c - '0' : c - 'a' + 10;

    temp = temp * 16 + c;
  }

  return value(temp);
}

} // namespace hls
