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

#ifndef _MEM_HPP_
#define _MEM_HPP_

#include "misc/types.hpp"

namespace hls
{

/**
 * @brief memcpy. Copies **bytes** bytes from src to dest.
 *
 * @param dest Pointer to where data will be copied to
 * @param src Source data pointer
 * @param bytes Length of data to be copied
 */
#ifndef __MEMCPY
#define __MEMCPY
#define memcpy(__destmmcpy, __srcmmcpy, __bytesmmcpy)                                                                  \
    [](void *dest, const void *src, size_t bytes) __attribute__((always_inline))                                       \
    {                                                                                                                  \
        const char *src_c = reinterpret_cast<const char *>(src);                                                       \
        char *dest_c = reinterpret_cast<char *>(dest);                                                                 \
        while (bytes--)                                                                                                \
        {                                                                                                              \
            *dest_c = *src_c;                                                                                          \
            ++src_c;                                                                                                   \
            ++dest_c;                                                                                                  \
        }                                                                                                              \
    }                                                                                                                  \
    (__destmmcpy, __srcmmcpy, __bytesmmcpy)
#endif

/**
 * @brief Copies **bytes** from src to dest, using a buffer in case regions
 * overlap
 *
 * @param dest Pointer to where data will be copied to
 * @param src Source data pointer
 * @param bytes
 * @return void* pointer to dest
 */
#ifndef __MEMMOVE
#define __MEMMOVE
#define memmove(__destmmov, __srcmmov, __bytesmmov)                                                                    \
    [](void *dest, const void *src, size_t bytes) -> void *__attribute__((always_inline))                              \
    {                                                                                                                  \
        const char *_src = reinterpret_cast<const char *>(src);                                                        \
        char *_dest = reinterpret_cast<char *>(dest);                                                                  \
                                                                                                                       \
        _dest = _dest + ((bytes - 1) * (_dest > _src));                                                                \
        _src = _src + ((bytes - 1) * (_dest > _src));                                                                  \
                                                                                                                       \
        for (size_t i = 0; i < bytes; ++i)                                                                             \
        {                                                                                                              \
            *_dest = *_src;                                                                                            \
            if (_dest > _src)                                                                                          \
            {                                                                                                          \
                --_dest;                                                                                               \
                --_src;                                                                                                \
            }                                                                                                          \
            else                                                                                                       \
            {                                                                                                          \
                ++_dest;                                                                                               \
                ++_src;                                                                                                \
            }                                                                                                          \
        }                                                                                                              \
        return dest;                                                                                                   \
    }                                                                                                                  \
    (__destmmov, __srcmmov, __bytesmmov)
#endif

/**
 * @brief Sets **n** bytes at **dest** to **c**
 *
 * @param dst Destination pointer.
 * @param c The value memory will be set to
 * @param size Length of memory to be set
 */
#ifndef __MEMSET
#define __MEMSET
#define memset(__srcmset, __cmset, __szmset)                                                                           \
    [](void *dst, char c, size_t size) __attribute__((always_inline))                                                  \
    {                                                                                                                  \
        byte *p = reinterpret_cast<byte *>(dst);                                                                       \
        for (size_t i = 0; i < size; ++i)                                                                              \
        {                                                                                                              \
            *p = c;                                                                                                    \
        }                                                                                                              \
    }                                                                                                                  \
    (__srcmset, __cmset, __szmset)
#endif

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
#ifndef __MEMCMP
#define __MEMCMP
#define memcmp(__ptr1mcmp, __ptr2mcmp, __nummcmp)                                                                      \
    [](const void *ptr1, const void *ptr2, size_t num) -> int __attribute__((always_inline))                           \
    {                                                                                                                  \
        const byte *p1 = reinterpret_cast<const byte *>(ptr1);                                                         \
        const byte *p2 = reinterpret_cast<const byte *>(ptr2);                                                         \
        byte a = 0;                                                                                                    \
        byte b = 0;                                                                                                    \
        for (size_t i = 0; i < num; ++i)                                                                               \
        {                                                                                                              \
            a = *(p1 + i);                                                                                             \
            b = *(p2 + i);                                                                                             \
            if (a != b)                                                                                                \
                break;                                                                                                 \
        }                                                                                                              \
        return a - b;                                                                                                  \
    }                                                                                                                  \
    (__ptr1mcmp, __ptr2mcmp, __nummcmp)
#endif

#ifndef __MEMCHR
#define __MEMCHR
#define memchr(__ptrmchr, __chmchr, __countmchr)                                                                       \
    [](const void *ptr, byte ch, size_t count) -> const void *__attribute__((always_inline))                           \
    {                                                                                                                  \
        if (ptr != nullptr)                                                                                            \
        {                                                                                                              \
            const char *p = reinterpret_cast<const char *>(ptr);                                                       \
            for (size_t i = 0; i < count; ++i)                                                                         \
            {                                                                                                          \
                if (*(p + i) == ch)                                                                                    \
                    return (p + i);                                                                                    \
            }                                                                                                          \
        }                                                                                                              \
        return nullptr;                                                                                                \
    }                                                                                                                  \
    (__ptrmchr, __chmchr, __countmchr)
#endif

/**
 * @brief Converts a pointer value to a unsingned integral
 *
 * @param ptr Pointer to be converted
 * @return uintptr_t Pointer as unsigned integral
 */
#ifndef __TO_UINTPTR
#define __TO_UINTPTR
#define to_uintptr_t(__ruintptr)                                                                                       \
    [](const void *p) -> uintptr_t __attribute__((always_inline))                                                      \
    {                                                                                                                  \
        return reinterpret_cast<uintptr_t>(p);                                                                         \
    }                                                                                                                  \
    (__ruintptr)
#endif

/**
 * @brief Converts a unsigned integral value to a pointer value
 *
 * @param v The value to be converted
 * @return void* Unsigned integral as pointer
 */
#ifndef __TO_PTR
#define __TO_PTR
#define to_ptr(__qtoptr)                                                                                               \
    [](uintptr_t v) -> void *__attribute__((always_inline))                                                            \
    {                                                                                                                  \
        return reinterpret_cast<void *>(v);                                                                            \
    }                                                                                                                  \
    (__qtoptr)
#endif

/**
 * @brief Converts any pointer type to a byte* type.
 *
 * @param v The pointer to be converted
 * @return byte* Pointer as byte* type
 */
#ifndef __AS_BYTE_PTR
#define __AS_BYTE_PTR
#define as_byte_ptr(__abptr)                                                                                           \
    [](const void *p) -> byte *__attribute__((always_inline))                                                          \
    {                                                                                                                  \
        return reinterpret_cast<byte *>(const_cast<void *>(p));                                                        \
    }                                                                                                                  \
    (__abptr)
#endif

/**
 * @brief Applies an offset (in bytes) to a pointer value
 *
 * @param ptr The pointer to apply an offset to
 * @param offset Offset in bytes
 * @return const void* Pointer with offset applied
 */
#ifndef __A_OFFSET
#define __A_OFFSET
#define apply_offset(__pao, __offao)                                                                                   \
    [](void *p, uintptr_t offset) -> void *__attribute__((always_inline))                                              \
    {                                                                                                                  \
        return reinterpret_cast<char *>(p) + offset;                                                                   \
    }                                                                                                                  \
    (__pao, __offao)
#endif

/**
 * @brief Checks if a pointer follows alignment **alignment**
 *
 * @param ptr Pointer value to check alignment
 * @param alignment Alignment value
 * @return true If the pointer is aligned
 * @return false If the pointer is not aligned
 */
#ifndef __ISALIGNED
#define __ISALIGNED
#define is_aligned(__ptria, __alignmentia)                                                                             \
    [](const void *ptr, size_t alignment) -> bool __attribute__((always_inline))                                       \
    {                                                                                                                  \
        uintptr_t p = to_uintptr_t(ptr);                                                                               \
        return (p / alignment * alignment) == p;                                                                       \
    }                                                                                                                  \
    (__ptria, __alignmentia)
#endif

/**
 * @brief Returns the absolute difference (in bytes) between two pointers.
 *
 * @param __ptra One of the pointers.
 * @param __ptrb The other pointer.
 * @return size_t The absolute difference between both pointers.
 */
#ifndef __ABS_PTR_DIFF
#define __ABS_PTR_DIFF
#define abs_ptr_diff(__ptraabsd, __ptrbabsd)                                                                           \
    [](const void *a, const void *b) -> size_t __attribute__((always_inline))                                          \
    {                                                                                                                  \
        const char *_a = reinterpret_cast<const char *>(a);                                                            \
        const char *_b = reinterpret_cast<const char *>(b);                                                            \
                                                                                                                       \
        if (_a > _b)                                                                                                   \
            return _a - _b;                                                                                            \
        return _b - _a;                                                                                                \
    }                                                                                                                  \
    (__ptraabsd, __ptrbabsd)
#endif

/**
 * @brief Aligns a pointer **ptr** to alignment. If the pointer is not already
 * aligned, it will align it to the next stride, that is, the return value > ptr
 *
 * @param __ptraf Pointer to be aligned
 * @param alignment The required alignment
 * @return void* The pointer aligned
 */
#ifndef __AFORWARD
#define __AFORWARD
#define align_forward(__ptraf, __alignmentaf)                                                                          \
    [](const void *ptr, size_t alignment) -> void *__attribute__((always_inline))                                      \
    {                                                                                                                  \
        uintptr_t p = to_uintptr_t(ptr);                                                                               \
        if (alignment <= 1 || is_aligned(ptr, alignment))                                                              \
            return to_ptr(p);                                                                                          \
        return to_ptr(p + p % alignment);                                                                              \
    }                                                                                                                  \
    (__ptraf, __alignmentaf)
#endif

/**
 * @brief Aligns a pointer **ptr** to alignment. If the pointer is not already
 * aligned, it will align it to the stride before, that is, the return value <
 * ptr
 *
 * @param ptr Pointer to be aligned
 * @param alignment The required alignment
 * @return void* The pointer aligned
 */
#ifndef __ABACK
#define __ABACK
#define align_back(__ptrab, __alignmentab)                                                                             \
    [](const void *ptr, size_t alignment) -> void *__attribute__((always_inline))                                      \
    {                                                                                                                  \
        uintptr_t p = to_uintptr_t(ptr);                                                                               \
        if (alignment <= 1 || is_aligned(ptr, alignment))                                                              \
            return to_ptr(p);                                                                                          \
        p = (p / alignment) * alignment;                                                                               \
        return to_ptr(p);                                                                                              \
    }                                                                                                                  \
    (__ptrab, __alignmentab)
#endif

#ifndef __BSWAP16
#define __BSWAP16
#define byteswap16(__x)                                                                                                \
    [](uint16_t v) -> uint16_t __attribute__((always_inline))                                                          \
    {                                                                                                                  \
        return (v >> 8 | v << 8);                                                                                      \
    }                                                                                                                  \
    (__x)
#endif

#ifndef __BSWAP32
#define __BSWAP32
#define byteswap32(__v)                                                                                                \
    [](uint32_t v) -> uint32_t __attribute__((always_inline))                                                          \
    {                                                                                                                  \
        return ((v & 0xFF) << 24) | ((v & 0xFF00) << 8) | ((v >> 8) & 0xFF00) | ((v >> 24) | 0xFF);                    \
    };
#endif

#ifndef __BSWAP64
#define __BSWAP64
#define byteswap64(__z)                                                                                                \
    [](uint64_t v) -> uint64_t __attribute__((always_inline))                                                          \
    {                                                                                                                  \
        return v;                                                                                                      \
    }                                                                                                                  \
    (__z)
#endif

/**
 * @brief Reads a big endian uint16_t value
 *
 * @param data The value to be read
 * @return uint16_t A native endian uint16_t
 */
#ifndef _U16RBE
#define _U16RBE
#define uint16_read_be(__data)                                                                                         \
    [](uint16_t data) -> uint16_t __attribute__((always_inline))                                                       \
    {                                                                                                                  \
        if constexpr (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)                                                       \
        {                                                                                                              \
            return byteswap16(data);                                                                                   \
        }                                                                                                              \
        return data;                                                                                                   \
    }                                                                                                                  \
    (__data)
#endif

/**
 * @brief Reads a big endian uint32_t value
 *
 * @param data The value to be read
 * @return uint16_t A native endian uint32_t
 */
#ifndef _U32RBE
#define _U32RBE
#define uint32_read_be(__data)                                                                                         \
    [](uint32_t data) -> uint32_t __attribute__((always_inline))                                                       \
    {                                                                                                                  \
        if constexpr (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)                                                       \
        {                                                                                                              \
            return byteswap32(data);                                                                                   \
        }                                                                                                              \
        return data;                                                                                                   \
    }                                                                                                                  \
    (__data)
#endif

/**
 * @brief Reads a big endian uint64_t value
 *
 * @param data The value to be read
 * @return uint16_t A native endian uint64_t
 */
#ifndef _U64RBE
#define _U64RBE
#define uint64_read_be(__data)                                                                                         \
    [](uint64_t data) -> uint64_t __attribute__((always_inline))                                                       \
    {                                                                                                                  \
        if constexpr (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)                                                       \
        {                                                                                                              \
            return byteswap64(data);                                                                                   \
        }                                                                                                              \
        return data;                                                                                                   \
    }                                                                                                                  \
    (__data)
#endif

#ifndef __GETNEEDEDPAGES
#define __GETNEEDEDPAGES
#define get_needed_pages(__mem, __memsize, __p_lvl)                                                                    \
    [](const void *mem, size_t size, FrameLevel lvl) -> auto __attribute__((always_inline))                            \
    {                                                                                                                  \
        size_t alignment = get_frame_alignment(lvl);                                                                   \
        byte *p = reinterpret_cast<byte *>(const_cast<void *>(mem));                                                   \
        byte *back = reinterpret_cast<byte *>(align_back(p, alignment));                                               \
        byte *forward = reinterpret_cast<byte *>(                                                                      \
            align_forward(reinterpret_cast<byte *>(const_cast<void *>(mem)) + size, alignment));                       \
        return (forward - back) / get_frame_size(lvl);                                                                 \
    }                                                                                                                  \
    (__mem, __memsize, __p_lvl)
#endif

} // namespace hls

#endif
