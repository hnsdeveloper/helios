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

#ifndef _LOCALE_H_
#define _LOCALE_H_

#include "klibc/kstdbool.h"
#include "klibc/kstddef.h"
#include "klibc/kuchar.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Checks if an ascii string can be converted to UTF-16
     *
     * @param str The string argument
     * @return true If it can be converted
     * @return false If it cannot be converted
     */
    bool is_ascii_utf16_convertible(const char *str);

    /**
     * @brief Checks if a UTF-16 string can be converted to ascii
     *
     * @param str The string argument
     * @return true If it can be converted
     * @return false If it cannot be converted
     */
    bool is_utf16_ascii_convertible(const char16_t *str);

    /**
     * @brief Calculates the minimum size a buffer must have to accommodate a string conversion from ascii to UTF-16.
     *
     * @param str The string
     * @return size_t The minimum size of the buffer or zero either when it is an empty string or a string that cannot
     * be converted.
     */
    size_t calc_min_buffer_ascii_to_utf16(const char *str);

    /**
     * @brief Calculates the minimum size a buffer must have to accommodate a string conversion from UTF-16 to ascii.
     *
     * @param str The string
     * @return size_t The minimum size of the buffer or zero either when it is an empty string or a string that cannot
     * be converted.
     */
    size_t calc_min_buffer_utf16_to_ascii(const char16_t *str);

    /**
     * @brief Converts an ascii string to UTF-16. If the string argument is not convertible, the function fails.
     *
     * @param str The string to be converted.
     * @param buffer The buffer where the converted string will be stored.
     * @param buffer_size The size in bytes of the buffer (must be an even number)
     * @return char16_t* NULL on failure or a pointer to the beggining of the converted string on success.
     */
    char16_t *ascii_to_utf16(const char *str, void *buffer, size_t buffer_size);

    /**
     * @brief Converts an UTF-16 string to ascii. If the string argument is not convertible, the function fails.
     *
     * @param str The string to be converted.
     * @param buffer The buffer where the converted string will be stored.
     * @param buffer_size The size in bytes of the buffer.
     * @return char16_t* NULL on failure or a pointer to the beggining of the converted string on success.
     */
    char *utf16_to_ascii(const char16_t *str, void *buffer, size_t buffer_size);

#ifdef __cplusplus
}
#endif

#endif
