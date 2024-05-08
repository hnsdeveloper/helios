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

#include "misc/types.hpp"
#include "ulib/result.hpp"

namespace hls {

/**
 * @brief Compares two strings lexicographically.
 *
 * @param str1 Null terminated string
 * @param str2 Null terminated string to compare against.
 * @return int 0 if both strings are equal (either lexicographically or both
 * nullptr), -1 if str1 < str2, 1 if str1 > str2.
 */
int strcmp(const char *str1, const char *str2);

/**
 * @brief Compares to strings lexicographically up to **n** characters.
 *
 * @param str1 Null terminated string.
 * @param str2 Null terminated string to compare against.
 * @param n How many characters to compare.
 * @return int 0 if both strings are equal (either lexicographically or both
 * nullptr), -1 if str1 < str2, 1 if str1 > str2.
 */
int strncmp(const char *str1, const char *str2, size_t n);

/**
 * @brief Counts how many characters a string has, excluding the null
 * terminator.
 *
 * @param str Null terminated string.
 * @return size_t String lenght.
 */
#ifndef __STRLEN
#define __STRLEN
#define strlen(__str)                                                                                                  \
    [](const char *s) -> size_t __attribute__((always_inline)) {                                                       \
        size_t i = 0;                                                                                                  \
        if (str)                                                                                                       \
            for (; str[i] != '\0'; ++i)                                                                                \
                ;                                                                                                      \
        return i;                                                                                                      \
    }                                                                                                                  \
    (str)
#endif

/**
 * @brief Count how many characters a string has, up to *maxlen* characters,
 * excluding the null terminator.
 *
 * @param str Null terminated string.
 * @param maxlen Maximum length.
 * @return size_t strlen(str) if strlen(str) < maxlen, otherwise maxlen
 */
size_t strnlen(const char *str, size_t maxlen);

/**
 * @brief Finds the first occurrence of a character **c** on a string.
 *
 * @param str The string to look for a character.
 * @param c The character.
 * @return const char* Returns a pointer to the character on success and nullptr
 * if the string doesn't contain the character
 */
const char *strchr(const char *str, char c);

/**
 * @brief Finds the last occurrence of a character **c** on a string.
 *
 * @param str The string to look for a character.
 * @param c The character.
 * @return const char* Returns a pointer to the character on success and nullptr
 * if the string doesn't contain the character
 */
const char *strrchr(const char *str, char c);

/**
 * @brief Converts a string containing a numerical value to unsigned long.
 *
 * @param str The string containing a numerical value.
 * @param endptr A pointer to a char pointer where on success, it will be
 * pointing to the first character representing the numerical value.
 * @param base The base of the numerical value.
 * @return unsigned long int 0 on fail together with *endptr being nullptr,
 * otherwise the numerical value.
 */
unsigned long int strtoul(const char *str, char **endptr, int base);

/**
 * @brief Transforms lowercase character into uppercase character.
 *
 * @param c Character to be transformed.
 * @return char Uppercase **c** if **c** is a character, otherwise **c** itself.
 */
char to_upper(char c);

/**
 * @brief Transforms uppercase character into lowercase character.
 *
 * @param c Character to be transformed.
 * @return char Lowercase **c** if **c** is a character, otherwise **c** itself.
 */
char to_lower(char c);

/**
 * @brief Checks if character **c** is within range '0' - '9' inclusive.
 *
 * @param c Character to be checked.
 * @return true **c** is in range.
 * @return false **c** is not in range.
 */
bool is_dec_digit(char c);

/**
 * @brief Checks if character **c** is within range '0' - '7' inclusive.
 *
 * @param c Character to be checked.
 * @return true **c** is in range.
 * @return false **c** is not in range.
 */
bool is_oct_digit(char c);

/**
 * @brief Checks if character **c** is within range '0' - '9' inclusive or 'a' -
 * 'f' inclusive.
 *
 * @param c Character to be checked.
 * @return true **c** is in range.
 * @return false **c** is not in range.
 */
bool is_hex_digit(char c);

/**
 * @brief Checks if character **c** is within range 'a' - 'z' inclusive or 'A' -
 * 'Z' inclusive.
 *
 * @param c Character to be checked.
 * @return true **c** is in range.
 * @return false **c** is not in range.
 */
bool is_alpha(char c);

/**
 * @brief Same as is_alpha(c) || is_dec_digit(c);
 *
 * @param c Character to be checked.
 * @return true **c** is in range.
 * @return false **c** is not in range.
 */
bool is_alphanumeric(char c);

/**
 * @brief Checks if character is same as ' ' (ASCII 32).
 *
 * @param c Character to be checked.
 * @return true **c** is in range.
 * @return false **c** is not in range.
 */
bool is_space(char c);

/**
 * @brief Checks if character is within range 'A' - 'Z' inclusive.
 *
 * @param c Character to be checked.
 * @return true **c** is in range.
 * @return false **c** is not in range.
 */
bool is_upper(char c);

/**
 * @brief Checks if character is within range 'a' - 'z' inclusive.
 *
 * @param c Character to be checked.
 * @return true **c** is in range.
 * @return false **c** is not in range.
 */
bool is_lower(char c);

/**
 * @brief Converts a string containing a hex value to a uint64_t.
 *
 * @param str String containing a hex value.
 * @return Result<uint64_t> Result<uint64_t> will contain a uint64_t or success
 * or an error value on error.
 */
Result<uint64_t> hex_to_uint(const char *str);

} // namespace hls

#endif