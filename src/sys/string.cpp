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

} // namespace hls
