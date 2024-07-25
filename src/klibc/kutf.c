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

#include "klibc/kutf.h"
#include "klibc/kstring.h"

bool is_ascii_utf16_convertible(const char *str)
{
    while (str && *str != 0)
    {
        unsigned char *c = (unsigned char *)(str);
        if (*c & 0x80)
            return false;
        ++str;
    }
    return true;
}

bool is_utf16_ascii_convertible(const char16_t *str)
{
    while (str && *str != 0)
    {
        if (*str & 0xFF80)
            return false;
        ++str;
    }
    return true;
}

size_t calc_min_buffer_ascii_to_utf16(const char *str)
{
    return strlen(str) ? strlen(str) * 2 + 2 : 0;
}

size_t calc_min_buffer_utf16_to_ascii(const char16_t *str)
{
    if (!is_utf16_ascii_convertible(str))
        return 0;
    return utf16_len(str) / 2 + 1;
}

char16_t *ascii_to_utf16(const char *src, void *buffer, size_t buffer_size)
{
    if (!src || !buffer || buffer_size == 0 || (buffer_size | 1) || !is_ascii_utf16_convertible(src))
        return NULL;

    char16_t *_buffer = (char16_t *)(buffer);
    size_t i = 0;
    while (src[i] != 0 && i < (buffer_size / 2) - 1)
    {
        _buffer[i] = ((unsigned char *)(src))[i];
        ++i;
    }
    _buffer[i] = 0;
    return _buffer;
}

char *utf16_to_ascii(const char16_t *src, void *buffer, size_t buffer_size)
{
    if (!src || !buffer || buffer_size == 0 || !is_utf16_ascii_convertible(src))
        return NULL;

    char *_buffer = buffer;
    size_t i = 0;
    while (src[i] != 0 && i < (buffer_size - 1))
    {
        unsigned char *p = (unsigned char *)(_buffer + i);
        *p = (src[i] & 0xFF80) ^ 0xFF80;
        ++i;
    }
    _buffer[i] = 0;
    return _buffer;
}
