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

#include "sys/mem.hpp"

namespace hls
{

    uintptr_t to_uintptr_t(const void *p)
    {
        return reinterpret_cast<uintptr_t>(p);
    }

    void *to_ptr(uintptr_t v)
    {
        return reinterpret_cast<void *>(v);
    }

    byte *as_byte_ptr(const void *p)
    {
        return reinterpret_cast<byte *>(const_cast<void *>(p));
    }

    void *apply_offset(void *p, uintptr_t offset)
    {
        return reinterpret_cast<char *>(p) + offset;
    }

    bool is_aligned(const void *ptr, size_t alignment)
    {
        uintptr_t p = to_uintptr_t(ptr);
        return (p / alignment * alignment) == p;
    }

    size_t abs_ptr_diff(const void *a, const void *b)
    {
        const char *_a = reinterpret_cast<const char *>(a);
        const char *_b = reinterpret_cast<const char *>(b);

        if (_a > _b)
            return _a - _b;
        return _b - _a;
    }

    void *align_forward(const void *ptr, size_t alignment)
    {
        if (!((alignment - 1) & alignment))
        {
            uintptr_t p = to_uintptr_t(ptr);
            return to_ptr((p + (alignment - 1)) & (~alignment + 1));
        }
        return const_cast<void *>(ptr);
    }

    void *align_back(const void *ptr, size_t alignment)
    {
        if (!((alignment - 1) & alignment) && ptr && (alignment > 1))
        {
            uintptr_t p = to_uintptr_t(ptr);
            return to_ptr(((p + (alignment - 1)) & (~alignment + 1)) - alignment);
        }

        return const_cast<void *>(ptr);
    }

    uint16_t byteswap16(uint16_t v)
    {
        return (v >> 8 | v << 8);
    }

    uint32_t byteswap32(uint32_t v)
    {
        return ((v & 0xFF) << 24) | ((v & 0xFF00) << 8) | ((v >> 8) & 0xFF00) | ((v >> 24) | 0xFF);
    }

    uint64_t byteswap64(uint64_t v)
    {
        return v;
    }

    uint16_t uint16_read_be(uint16_t data)
    {
        if constexpr (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
        {
            return byteswap16(data);
        }
        return data;
    }

    uint32_t uint32_read_be(uint32_t data)
    {
        if constexpr (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
        {
            return byteswap32(data);
        }
        return data;
    }

    uint64_t uint64_read_be(uint64_t data)
    {
        if constexpr (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
        {
            return byteswap64(data);
        }
        return data;
    }

    size_t get_needed_pages(const void *mem, size_t size, FrameOrder lvl)
    {
        size_t alignment = get_frame_alignment(lvl);
        byte *p = reinterpret_cast<byte *>(const_cast<void *>(mem));
        byte *back = reinterpret_cast<byte *>(align_back(p, alignment));
        byte *forward = reinterpret_cast<byte *>(
            align_forward(reinterpret_cast<byte *>(const_cast<void *>(mem)) + size, alignment));
        return (forward - back) / get_frame_size(lvl);
    }

    bool is_power_of_two(size_t val)
    {
        return !(val & (val - 1));
    }

} // namespace hls
