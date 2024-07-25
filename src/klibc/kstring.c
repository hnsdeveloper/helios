/*
 * string.c
 *
 * Copyright (C) 2021 bzt (bztsrc@gitlab)
 * Copyright (C) 2024 Helio Nunes Santos
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * This file is originally part of the POSIX-UEFI package with additions tailored to HeliOS package.
 * @brief Implementing functions which are defined in kstring.h
 *
 */

#include <klibc/kstring.h>

void *memcpy(void *dst, const void *src, size_t n)
{
    uint8_t *a = (uint8_t *)dst, *b = (uint8_t *)src;
    if (src && dst && src != dst && n > 0)
    {
        while (n--)
            *a++ = *b++;
    }
    return dst;
}

void *memmove(void *dst, const void *src, size_t n)
{
    uint8_t *a = (uint8_t *)dst, *b = (uint8_t *)src;
    if (src && dst && src != dst && n > 0)
    {
        if (a > b && a < b + n)
        {
            a += n - 1;
            b += n - 1;
            while (n-- > 0)
                *a-- = *b--;
        }
        else
        {
            while (n--)
                *a++ = *b++;
        }
    }
    return dst;
}

void *memset(void *s, int c, size_t n)
{
    uint8_t *p = (uint8_t *)s;
    if (s && n > 0)
    {
        while (n--)
            *p++ = (uint8_t)c;
    }
    return s;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
    uint8_t *a = (uint8_t *)s1, *b = (uint8_t *)s2;
    if (s1 && s2 && s1 != s2 && n > 0)
    {
        while (n--)
        {
            if (*a != *b)
                return *a - *b;
            a++;
            b++;
        }
    }
    return 0;
}

void *memchr(const void *s, int c, size_t n)
{
    uint8_t *e, *p = (uint8_t *)s;
    if (s && n > 0)
    {
        for (e = p + n; p < e; p++)
            if (*p == (uint8_t)c)
                return p;
    }
    return NULL;
}

void *memrchr(const void *s, int c, size_t n)
{
    uint8_t *e, *p = (uint8_t *)s;
    if (s && n > 0)
    {
        for (e = p + n; p < e; --e)
            if (*e == (uint8_t)c)
                return e;
    }
    return NULL;
}

void *memmem(const void *haystack, size_t hl, const void *needle, size_t nl)
{
    uint8_t *c = (uint8_t *)haystack;
    if (!haystack || !needle || !hl || !nl || nl > hl)
        return NULL;
    hl -= nl - 1;
    while (hl)
    {
        if (!memcmp(c, needle, nl))
            return c;
        c++;
        hl--;
    }
    return NULL;
}

void *memrmem(const void *haystack, size_t hl, const void *needle, size_t nl)
{
    uint8_t *c = (uint8_t *)haystack;
    if (!haystack || !needle || !hl || !nl || nl > hl)
        return NULL;
    hl -= nl;
    c += hl;
    while (hl)
    {
        if (!memcmp(c, needle, nl))
            return c;
        c--;
        hl--;
    }
    return NULL;
}

char *strcpy(char *dst, const char *src)
{
    char *s = dst;
    if (src && dst && src != dst)
    {
        while (*src)
        {
            *dst++ = *src++;
        }
        *dst = 0;
    }
    return s;
}

char *strncpy(char *dst, const char *src, size_t n)
{
    char *s = dst;
    const char *e = src + n;
    if (src && dst && src != dst && n > 0)
    {
        while (*src && src < e)
        {
            *dst++ = *src++;
        }
        *dst = 0;
    }
    return s;
}

char *strcat(char *dst, const char *src)
{
    char *s = dst;
    if (src && dst)
    {
        dst += strlen(dst);
        while (*src)
        {
            *dst++ = *src++;
        }
        *dst = 0;
    }
    return s;
}

int strcmp(const char *s1, const char *s2)
{
    if (s1 && s2 && s1 != s2)
    {
        while (*s1 && *s1 == *s2)
        {
            s1++;
            s2++;
        }
        return *s1 - *s2;
    }
    return 0;
}

char *strncat(char *dst, const char *src, size_t n)
{
    char *s = dst;
    const char *e = src + n;
    if (src && dst && n > 0)
    {
        dst += strlen(dst);
        while (*src && src < e)
        {
            *dst++ = *src++;
        }
        *dst = 0;
    }
    return s;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
    const char *e = s1 + n - 1;
    if (s1 && s2 && s1 != s2 && n > 0)
    {
        while (s1 < e && *s1 && *s1 == *s2)
        {
            s1++;
            s2++;
        }
        return *s1 - *s2;
    }
    return 0;
}

const char *strchr(const char *s, int c)
{
    if (s)
    {
        while (*s)
        {
            if (*s == (char)c)
                return (char *)s;
            s++;
        }
    }
    return NULL;
}

char *strrchr(const char *s, int c)
{
    char *e;
    if (s)
    {
        e = (char *)s + strlen(s) - 1;
        while (s <= e)
        {
            if (*e == (char)c)
                return e;
            e--;
        }
    }
    return NULL;
}

char *strstr(const char *haystack, const char *needle)
{
    return memmem(haystack, strlen(haystack) * sizeof(char), needle, strlen(needle) * sizeof(char));
}

static char *_strtok_r(char *s, const char *d, char **p)
{
    int c, sc;
    char *tok, *sp;

    if (d == NULL || (s == NULL && (s = *p) == NULL))
        return NULL;
again:
    c = *s++;
    for (sp = (char *)d; (sc = *sp++) != 0;)
    {
        if (c == sc)
            goto again;
    }

    if (c == 0)
    {
        *p = NULL;
        return NULL;
    }
    tok = s - 1;
    while (1)
    {
        c = *s++;
        sp = (char *)d;
        do
        {
            if ((sc = *sp++) == c)
            {
                if (c == 0)
                    s = NULL;
                else
                    *(s - 1) = 0;
                *p = s;
                return tok;
            }
        } while (sc != 0);
    }
    return NULL;
}

char *strtok(char *s, const char *delim)
{
    static char *p;
    return _strtok_r(s, delim, &p);
}

char *strtok_r(char *s, const char *delim, char **ptr)
{
    return _strtok_r(s, delim, ptr);
}

size_t strlen(const char *__s)
{
    size_t ret;

    if (!__s)
        return 0;
    for (ret = 0; __s[ret]; ret++)
        ;
    return ret;
}

size_t strnlen(const char *str, size_t maxlen)
{
    size_t i = 0;
    if (str)
    {
        while (i < maxlen && *str != '\0')
        {
            ++i;
        }
    }
    return i;
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