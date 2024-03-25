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

#ifndef _STRING_FUNCTIONS_HPP_
#define _STRING_FUNCTIONS_HPP_

#include "include/types.h"
#include "ulib/result.hpp"

namespace hls {

int strcmp(const char *str1, const char *str2);
int strncmp(const char *str1, const char *str2, size_t n);
size_t strlen(const char *str);
size_t strnlen(const char *str, size_t maxlen);
const char *strchr(const char *str, char c);
const char *strrchr(const char *str, char c);
unsigned long int strtoul(const char *str, char **endptr, int base);

Result<size_t> find_first_idx(const char *str, char to_find);

char to_upper(char c);
char to_lower(char c);

bool is_dec_digit(char c);
bool is_oct_digit(char c);
bool is_hex_digit(char c);

bool is_alpha(char c);
bool is_alphanumeric(char c);
bool is_space(char c);

bool is_upper(char c);
bool is_lower(char c);

Result<uint64_t> hex_to_uint(const char *str);

} // namespace hls

#endif