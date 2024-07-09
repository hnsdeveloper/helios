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

namespace hls
{

    bool isspace(char c)
    {
        return c == ' ';
    }

    bool isupper(char c)
    {
        return c >= 'A' && c <= 'Z';
    }

    bool islower(char c)
    {
        return c >= 'a' && c <= 'z';
    }

    bool toupper(char c)
    {
        if (islower(c))
            return c - 'a' + 'A';
        return c;
    }

    char tolower(char c)
    {
        if (isupper(c))
            return c - 'A' + 'a';
        return c;
    }

    bool isdec(char c)
    {
        return c >= '0' && c <= '9';
    }

    bool isoct(char c)
    {
        return c >= '0' && c <= '7';
    }

    bool ishex(char c)
    {
        c = tolower(c);
        return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
    }

    bool isalpha(char c)
    {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }

    bool isalphanumeric(char c)
    {
        return isdec(c) || isalpha(c);
    }

    int strcmp(const char *str1, const char *str2)
    {
        while (*str1 && *str2)
        {
            if (*str1 != *str2)
                break;
            ++str1;
            ++str2;
        }
        return *str1 - *str2;
    }

    int strncmp(const char *str1, const char *str2, size_t n)
    {
        char a = 0, b = 0;
        size_t i = 0;
        while (str1 && str2 && i++ < n)
        {
            a = *str1;
            b = *str2;
            if (*str1 != *str2 || !(*str1) || !(*str2))
                break;
            ++str1;
            ++str2;
        }
        return a - b;
    }

    int strlen(const char *str)
    {
        int i = 0;
        if (str)
            for (; str[i] != '\0'; ++i)
                ;
        return i;
    }

    int strnlen(const char *str, int maxlen)
    {
        int i = 0;
        if (str)
        {
            while (i < maxlen && *str != '\0')
            {
                ++i;
            }
        }
        return i;
    }

    const char *strchr(const char *str, char c)
    {
        while (true)
        {
            char v = *str;
            if (v == '\0')
                return nullptr;
            if (v == c)
                break;
            ++str;
        }
        return str;
    }

    const char *strrchr(const char *str, char c)
    {
        if (str)
        {
            size_t idx = 0;
            for (size_t i = 0; *(str + i); ++i)
            {
                char z = *(str + i);
                if (z == c)
                    idx = i;
            }
            if (idx || *str == c)
                return str + idx;
        }
        return nullptr;
    }

    unsigned long strtoul(const char *nptr, char **endptr, int base)
    {
        const unsigned long ULONG_MAX = (0ul - 1ul);
        const char *s;
        unsigned long acc, cutoff;
        int c;
        int neg, any, cutlim;
        s = nptr;
        do
        {
            c = (unsigned char)*s++;
        } while (isspace(c));
        if (c == '-')
        {
            neg = 1;
            c = *s++;
        }
        else
        {
            neg = 0;
            if (c == '+')
                c = *s++;
        }
        if ((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X'))
        {
            c = s[1];
            s += 2;
            base = 16;
        }
        if (base == 0)
            base = c == '0' ? 8 : 10;
        cutoff = ULONG_MAX / (unsigned long)base;
        cutlim = ULONG_MAX % (unsigned long)base;
        for (acc = 0, any = 0;; c = (unsigned char)*s++)
        {
            if (isdec(c))
                c -= '0';
            else if (isalpha(c))
                c -= isupper(c) ? 'A' - 10 : 'a' - 10;
            else
                break;
            if (c >= base)
                break;
            if (any < 0)
                continue;
            if (acc > cutoff || (acc == cutoff && c > cutlim))
            {
                any = -1;
                acc = ULONG_MAX;
            }
            else
            {
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

} // namespace hls
