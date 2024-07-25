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

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Compares two strings lexicographically.
 *
 * @param str1 Null terminated string
 * @param str2 Null terminated string to compare against.
 * @return int 0 if both strings are equal (either lexicographically or both
 * nullptr), negative value if str1 < str2, positive value if str1 > str2.
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
size_t strlen(const char *str);

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
const char *strchr(const char *str, int c);

/**
 * @brief memcpy. Copies **n** bytes from src to dest.
 *
 * @param dest Pointer to where data will be copied to
 * @param src Source data pointer
 * @param n Length of data to be copied
 */
void *memcpy(void *dst, const void *src, size_t n);

/**
 * @brief Copies **bytes** from src to dest, using a buffer in case regions
 * overlap
 *
 * @param dest Pointer to where data will be copied to
 * @param src Source data pointer
 * @param bytes The size of the data to be moved
 * @return void* pointer to dest
 */
void *memmove(void *dst, const void *src, size_t n);

/**
 * @brief Sets **n** bytes at **dest** to **c**
 *
 * @param dst Destination pointer.
 * @param c The value memory will be set to
 * @param size Length of memory to be set
 */

void *memset(void *s, int c, size_t n);

/**
 * @brief Compares **n** bytes at **ptr1** and **ptr2** and returns which is
 * bigger
 *
 * @param ptr1 Pointer to memory region to be compared
 * @param ptr2 Pointer to memory region to be compared
 * @param num Length in bytes to compare
 * @return int 0 if both regions have the same data, -1 if the data at ptr1 <
 * ptr2, 1 if the data at ptr1 > ptr2
 */
int memcmp(const void *s1, const void *s2, size_t n);

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
unsigned long strtoul(const char *nptr, char **endptr, int base);

void *memchr(const void *s, int c, size_t n);

void *memrchr(const void *s, int c, size_t n);

void *memmem(const void *haystack, size_t hl, const void *needle, size_t nl);

void *memrmem(const void *haystack, size_t hl, const void *needle, size_t nl);

char *strcpy(char *dst, const char *src);

char *strncpy(char *dst, const char *src, size_t n);

char *strcat(char *dst, const char *src);

char *strncat(char *dst, const char *src, size_t n);

char *strrchr(const char *s, int c);

char *strstr(const char *haystack, const char *needle);

char *strtok(char *s, const char *delim);

char *strtok_r(char *s, const char *delim, char **ptr);