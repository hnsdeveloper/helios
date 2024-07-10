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

#include "sys/print.hpp"
#include "arch/riscv64gc/plat_def.hpp"
#include "sys/mem.hpp"

namespace hls
{

    void putchar(char c)
    {
        kinit_putchar(c);
    }

    void strprint(const char *str)
    {
        if (str)
        {
            while (*str)
            {
                putchar(*str);
                ++str;
            }
        }
    }

    void strprintln(const char *str)
    {
        strprint(str);
        putchar('\r');
        putchar('\n');
    }

    void ptrprint(const void *ptr)
    {
        auto v = to_uintptr_t(ptr);
        char buffer[sizeof(v) * 2];
        for (size_t i = 0; i < (sizeof(v) * 2); ++i)
        {
            char c = v & 0xF;
            if (c <= 9)
                c += '0';
            else
                c += 'A' - 10;
            buffer[i] = c;
            v = v >> 4;
        }
        putchar('0');
        putchar('x');
        for (size_t i = 0; i < sizeof(v) * 8 / 4; ++i)
        {
            char &c = buffer[sizeof(v) * 8 / 4 - i - 1];
            putchar(c);
        }
    }

    void intprint(int64_t v)
    {
        if (v < 0)
        {
            putchar('-');
            if (v < 9)
                uintprint(-(v / 10));

            putchar(-(v % 10) + '0');
        }
        else
        {
            uintprint(v);
        }
    }

    void uintprint(uint64_t v)
    {
        if (v > 9)
        {
            uintprint(v / 10);
        }
        char c = v % 10;
        c += '0';
        putchar(c);
    }

    void doubleprint(double val);

} // namespace hls
