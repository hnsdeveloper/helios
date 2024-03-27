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
  if (str)
    for (; str[i] != '\0'; ++i)
      ;
  return i;
}

size_t strnlen(const char *str, size_t maxlen) {
  size_t i = 0;
  if (str) {
    while (i < maxlen && *str != '\0') {
      ++i;
    }
  }
  return i;
}

Result<size_t> find_first_idx(const char *str, char to_find) {
  if (str) {
    for (size_t i = 0; *(str + i); ++i) {
      char c = *(str + i);
      if (to_find == c)
        return value(i);
    }
  }

  return error<size_t>(Error::NOT_FOUND);
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

const char *strrchr(const char *str, char c) {
  if (str) {
    size_t idx = 0;
    for (size_t i; *(str + i); ++i) {
      char z = *(str + i);
      if (z == c)
        idx = i;
    }

    if (idx || *str == c)
      return str + idx;
  }

  return nullptr;
}

// TODO: DOES NOT BEHAVE ENTIRELY LIKE THE C STANDARD ONE
// THUS ANY WHERE IT IS USED, THE CODE SHOULD BE REWORKED
unsigned long strtoul(const char *nptr, char **endptr, int base) {
  const unsigned long ULONG_MAX = (0ul - 1ul);

  const char *s;
  unsigned long acc, cutoff;
  int c;
  int neg, any, cutlim;
  /*
   * See strtol for comments as to the logic used.
   */
  s = nptr;
  do {
    c = (unsigned char)*s++;
  } while (is_space(c));
  if (c == '-') {
    neg = 1;
    c = *s++;
  } else {
    neg = 0;
    if (c == '+')
      c = *s++;
  }
  if ((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X')) {
    c = s[1];
    s += 2;
    base = 16;
  }
  if (base == 0)
    base = c == '0' ? 8 : 10;
  cutoff = ULONG_MAX / (unsigned long)base;
  cutlim = ULONG_MAX % (unsigned long)base;
  for (acc = 0, any = 0;; c = (unsigned char)*s++) {
    if (is_dec_digit(c))
      c -= '0';
    else if (is_alpha(c))
      c -= is_upper(c) ? 'A' - 10 : 'a' - 10;
    else
      break;
    if (c >= base)
      break;
    if (any < 0)
      continue;
    if (acc > cutoff || (acc == cutoff && c > cutlim)) {
      any = -1;
      acc = ULONG_MAX;
    } else {
      any = 1;
      acc *= (unsigned long)base;
      acc += c;
    }
  }
  if (neg && any > 0)
    acc = -acc;
  if (endptr != 0)
    *endptr = (char *)(any ? s - 1 : nptr);
  return (acc);
}

char to_upper(char c) {
  if (is_lower(c))
    return c - 'a' + 'A';

  return c;
}

char to_lower(char c) {
  if (is_upper(c))
    return c - 'A' + 'a';

  return c;
}

bool is_dec_digit(char c) {
  c = to_lower(c);
  return c >= '0' && c <= '9';
}
bool is_oct_digit(char c) {
  c = to_lower(c);
  return c >= '0' && c <= '7';
}

bool is_hex_digit(char c) {
  c = to_lower(c);
  return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
}

bool is_alpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
};

bool is_alphanumeric(char c) { return is_dec_digit(c) || is_alpha(c); }

Result<uint64_t> hex_to_uint(const char *str) {
  if (str == nullptr)
    return error<uint64_t>(Error::INVALID_ARGUMENT);

  if (*str == '0' && to_lower(*(str + 1)) == 'x')
    str = str + 2;

  uint64_t temp = 0;
  for (; *str; ++str) {
    char c = to_lower(*str);
    if (!is_hex_digit(c)) {
      return error<uint64_t>(Error::INVALID_ARGUMENT);
    }

    c = c <= '9' ? c - '0' : c - 'a' + 10;

    temp = temp * 16 + c;
  }

  return value(temp);
}

bool is_space(char c) { return c == ' '; }

bool is_upper(char c) { return to_upper(c) == c; }
bool is_lower(char c) { return to_lower(c) == c; };

} // namespace hls
