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

namespace hls
{

/**
 * @brief Checks if character is same as ' ' (ASCII 32).
 *
 * @param c Character to be checked.
 * @return true **c** is in range.
 * @return false **c** is not in range.
 */
#ifndef __ISSPACE
#define __ISSPACE
#define isspace(__c)                                                                                                   \
    [](char c) -> bool __attribute__((always_inline))                                                                  \
    {                                                                                                                  \
        return c == ' ';                                                                                               \
    }                                                                                                                  \
    (__c)
#endif

/**
 * @brief Checks if character is within range 'A' - 'Z' inclusive.
 *
 * @param c Character to be checked.
 * @return true **c** is in range.
 * @return false **c** is not in range.
 */
#ifndef __ISUPPER
#define __ISUPPER
#define isupper(__c)                                                                                                   \
    [](char c) -> bool __attribute__((always_inline))                                                                  \
    {                                                                                                                  \
        return c >= 'A' && c <= 'Z';                                                                                   \
    }                                                                                                                  \
    (__c)
#endif

/**
 * @brief Checks if character is within range 'a' - 'z' inclusive.
 *
 * @param c Character to be checked.
 * @return true **c** is in range.
 * @return false **c** is not in range.
 */
#ifndef __ISLOWER
#define __ISLOWER
#define islower(__c)                                                                                                   \
    [](char c) -> bool __attribute__((always_inline))                                                                  \
    {                                                                                                                  \
        return c >= 'a' && c <= 'z';                                                                                   \
    }                                                                                                                  \
    (__c)
#endif

/**
 * @brief Transforms lowercase character into uppercase character.
 *
 * @param c Character to be transformed.
 * @return char Uppercase **c** if **c** is a character, otherwise **c** itself.
 */
#ifndef __TOUPPER
#define __TOUPPER
#define toupper(__c)                                                                                                   \
    [](char c) -> char __attribute__((always_inline))                                                                  \
    {                                                                                                                  \
        if (islower(c))                                                                                                \
            return c - 'a' + 'A';                                                                                      \
        return c;                                                                                                      \
    }                                                                                                                  \
    (__c)
#endif

/**
 * @brief Transforms uppercase character into lowercase character.
 *
 * @param c Character to be transformed.
 * @return char Lowercase **c** if **c** is a character, otherwise **c** itself.
 */
#ifndef __TOLOWER
#define __TOLOWER
#define tolower(__c)                                                                                                   \
    [](char c) -> char __attribute__((always_inline))                                                                  \
    {                                                                                                                  \
        if (isupper(c))                                                                                                \
            return c - 'A' + 'a';                                                                                      \
        return c;                                                                                                      \
    }                                                                                                                  \
    (__c)
#endif

/**
 * @brief Checks if character **c** is within range '0' - '9' inclusive.
 *
 * @param c Character to be checked.
 * @return true **c** is in range.
 * @return false **c** is not in range.
 */
#ifndef __ISDEC
#define __ISDEC
#define isdec(__c)                                                                                                     \
    [](char c) -> bool __attribute__((always_inline))                                                                  \
    {                                                                                                                  \
        return c >= '0' && c <= '9';                                                                                   \
    }                                                                                                                  \
    (__c)
#endif

/**
 * @brief Checks if character **c** is within range '0' - '7' inclusive.
 *
 * @param c Character to be checked.
 * @return true **c** is in range.
 * @return false **c** is not in range.
 */
#ifndef __ISOCT
#define __ISOCT
#define isoct(__c)                                                                                                     \
    [](char c) -> bool __attribute__((always_inline))                                                                  \
    {                                                                                                                  \
        return c >= '0' && c <= '7';                                                                                   \
    }                                                                                                                  \
    (__c)
#endif

/**
 * @brief Checks if character **c** is within range '0' - '9' inclusive or 'a' -
 * 'f' inclusive.
 *
 * @param c Character to be checked.
 * @return true **c** is in range.
 * @return false **c** is not in range.
 */
#ifndef __ISHEX
#define __ISHEX
#define ishex(__c)                                                                                                     \
    [](char c) -> bool __attribute__((always_inline))                                                                  \
    {                                                                                                                  \
        c = tolower(c);                                                                                                \
        return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');                                                       \
    }                                                                                                                  \
    (__c)
#endif

/**
 * @brief Checks if character **c** is within range 'a' - 'z' inclusive or 'A' -
 * 'Z' inclusive.
 *
 * @param c Character to be checked.
 * @return true **c** is in range.
 * @return false **c** is not in range.
 */
#ifndef __ISALPHA
#define __ISALPHA
#define isalpha(__c)                                                                                                   \
    [](char c) -> bool __attribute__((always_inline))                                                                  \
    {                                                                                                                  \
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');                                                       \
    }                                                                                                                  \
    (__c)
#endif

/**
 * @brief Same as is_alpha(c) || is_dec_digit(c);
 *
 * @param c Character to be checked.
 * @return true **c** is in range.
 * @return false **c** is not in range.
 */
#ifndef __ISALPHANUM
#define __ISALPHANUM
#define isalphanumeric(__c)                                                                                            \
    [](char c) -> bool __attribute__((always_inline))                                                                  \
    {                                                                                                                  \
        return isdec(c) || isalpha(c);                                                                                 \
    }                                                                                                                  \
    (__c)
#endif

/**
 * @brief Compares two strings lexicographically.
 *
 * @param str1 Null terminated string
 * @param str2 Null terminated string to compare against.
 * @return int 0 if both strings are equal (either lexicographically or both
 * nullptr), negative value if str1 < str2, positive value if str1 > str2.
 */
#ifndef __STRCMP
#define __STRCMP
#define strcmp(__str1, __str2)                                                                                         \
    [](const char *str1, const char *str2) -> int __attribute__((always_inline))                                       \
    {                                                                                                                  \
        while (*str1 && *str2)                                                                                         \
        {                                                                                                              \
            if (*str1 != *str2)                                                                                        \
                break;                                                                                                 \
            ++str1;                                                                                                    \
            ++str2;                                                                                                    \
        }                                                                                                              \
        return *str1 - *str2;                                                                                          \
    }                                                                                                                  \
    (__str1, __str2)
#endif

/**
 * @brief Compares to strings lexicographically up to **n** characters.
 *
 * @param str1 Null terminated string.
 * @param str2 Null terminated string to compare against.
 * @param n How many characters to compare.
 * @return int 0 if both strings are equal (either lexicographically or both
 * nullptr), -1 if str1 < str2, 1 if str1 > str2.
 */
#ifndef __STRNCMP
#define __STRNCMP
#define strncmp(__str1, __str2, __n)                                                                                   \
    [](const char *str1, const char *str2, size_t n) -> int __attribute__((always_inline))                             \
    {                                                                                                                  \
        char a = 0, b = 0;                                                                                             \
        size_t i = 0;                                                                                                  \
        while (str1 && str2 && i++ < n)                                                                                \
        {                                                                                                              \
            if (*str1 != *str2 || !(*str1) || !(*str2))                                                                \
                break;                                                                                                 \
            ++str1;                                                                                                    \
            ++str2;                                                                                                    \
        }                                                                                                              \
        return a - b;                                                                                                  \
    }                                                                                                                  \
    (__str1, __str2, __n)
#endif

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
    [](const char *str) -> size_t __attribute__((always_inline))                                                       \
    {                                                                                                                  \
        size_t i = 0;                                                                                                  \
        if (str)                                                                                                       \
            for (; str[i] != '\0'; ++i)                                                                                \
                ;                                                                                                      \
        return i;                                                                                                      \
    }                                                                                                                  \
    (__str)
#endif

/**
 * @brief Count how many characters a string has, up to *maxlen* characters,
 * excluding the null terminator.
 *
 * @param str Null terminated string.
 * @param maxlen Maximum length.
 * @return size_t strlen(str) if strlen(str) < maxlen, otherwise maxlen
 */
#ifndef __STRNLEN
#define __STRNLEN
#define strnlen(__str, __maxlen)                                                                                       \
    [](const char *str, size_t maxlen) -> size_t __attribute__((always_inline))                                        \
    {                                                                                                                  \
        size_t i = 0;                                                                                                  \
        if (str)                                                                                                       \
        {                                                                                                              \
            while (i < maxlen && *str != '\0')                                                                         \
            {                                                                                                          \
                ++i;                                                                                                   \
            }                                                                                                          \
        }                                                                                                              \
        return i;                                                                                                      \
    }                                                                                                                  \
    (__str, __maxlen)
#endif

/**
 * @brief Finds the first occurrence of a character **c** on a string.
 *
 * @param str The string to look for a character.
 * @param c The character.
 * @return const char* Returns a pointer to the character on success and nullptr
 * if the string doesn't contain the character
 */
#ifndef __STRCHR
#define __STRCHR
#define strchr(__str, __c)                                                                                             \
    [](const char *str, char c) -> const char *__attribute__((always_inline))                                          \
    {                                                                                                                  \
        while (true)                                                                                                   \
        {                                                                                                              \
            char v = *str;                                                                                             \
            if (v == '\0')                                                                                             \
                return nullptr;                                                                                        \
            if (v == c)                                                                                                \
                break;                                                                                                 \
            ++str;                                                                                                     \
        }                                                                                                              \
        return str;                                                                                                    \
    }                                                                                                                  \
    (__str, __c)
#endif

/**
 * @brief Finds the last occurrence of a character **c** on a string.
 *
 * @param str The string to look for a character.
 * @param c The character.
 * @return const char* Returns a pointer to the character on success and nullptr
 * if the string doesn't contain the character
 */
#ifndef __STRRCHR
#define __STRRCHR
#define strrchr(__str, __c)                                                                                            \
    [](const char *str, char c) -> const char *__attribute__((always_inline))                                          \
    {                                                                                                                  \
        if (str)                                                                                                       \
        {                                                                                                              \
            size_t idx = 0;                                                                                            \
            for (size_t i; *(str + i); ++i)                                                                            \
            {                                                                                                          \
                char z = *(str + i);                                                                                   \
                if (z == c)                                                                                            \
                    idx = i;                                                                                           \
            }                                                                                                          \
            if (idx || *str == c)                                                                                      \
                return str + idx;                                                                                      \
        }                                                                                                              \
        return nullptr;                                                                                                \
    }                                                                                                                  \
    (__str, __c)
#endif

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
#ifndef __STRTOUL
#define __STRTOUL
#define strtoul(__nptr, __endptr, __base)                                                                              \
    [](const char *nptr, char **endptr, int base) -> unsigned long __attribute__((always_inline))                      \
    {                                                                                                                  \
        const unsigned long ULONG_MAX = (0ul - 1ul);                                                                   \
        const char *s;                                                                                                 \
        unsigned long acc, cutoff;                                                                                     \
        int c;                                                                                                         \
        int neg, any, cutlim;                                                                                          \
        s = nptr;                                                                                                      \
        do                                                                                                             \
        {                                                                                                              \
            c = (unsigned char)*s++;                                                                                   \
        } while (isspace(c));                                                                                          \
        if (c == '-')                                                                                                  \
        {                                                                                                              \
            neg = 1;                                                                                                   \
            c = *s++;                                                                                                  \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            neg = 0;                                                                                                   \
            if (c == '+')                                                                                              \
                c = *s++;                                                                                              \
        }                                                                                                              \
        if ((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X'))                                         \
        {                                                                                                              \
            c = s[1];                                                                                                  \
            s += 2;                                                                                                    \
            base = 16;                                                                                                 \
        }                                                                                                              \
        if (base == 0)                                                                                                 \
            base = c == '0' ? 8 : 10;                                                                                  \
        cutoff = ULONG_MAX / (unsigned long)base;                                                                      \
        cutlim = ULONG_MAX % (unsigned long)base;                                                                      \
        for (acc = 0, any = 0;; c = (unsigned char)*s++)                                                               \
        {                                                                                                              \
            if (isdec(c))                                                                                              \
                c -= '0';                                                                                              \
            else if (isalpha(c))                                                                                       \
                c -= isupper(c) ? 'A' - 10 : 'a' - 10;                                                                 \
            else                                                                                                       \
                break;                                                                                                 \
            if (c >= base)                                                                                             \
                break;                                                                                                 \
            if (any < 0)                                                                                               \
                continue;                                                                                              \
            if (acc > cutoff || (acc == cutoff && c > cutlim))                                                         \
            {                                                                                                          \
                any = -1;                                                                                              \
                acc = ULONG_MAX;                                                                                       \
            }                                                                                                          \
            else                                                                                                       \
            {                                                                                                          \
                any = 1;                                                                                               \
                acc *= (unsigned long)base;                                                                            \
                acc += c;                                                                                              \
            }                                                                                                          \
        }                                                                                                              \
        if (neg && any > 0)                                                                                            \
            acc = -acc;                                                                                                \
        if (endptr != 0)                                                                                               \
            *endptr = (char *)(any ? s - 1 : nptr);                                                                    \
        return (acc);                                                                                                  \
    }                                                                                                                  \
    (__nptr, __endptr, __base)
#endif

} // namespace hls

#endif
