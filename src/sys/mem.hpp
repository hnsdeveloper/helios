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

namespace hls {

/**
 * @brief memcpy. Copies **bytes** bytes from src to dest.
 *
 * @param dest Pointer to where data will be copied to
 * @param src Source data pointer
 * @param bytes Length of data to be copied
 */
#ifndef __MEMCPY
#define __MEMCPY
#define memcpy(__dest, __src, __bytes)                                                                                 \
    [](void *dest, const void *src, size_t bytes) __attribute__((always_inline)) {                                     \
        const char *src_c = reinterpret_cast<const char *>(src);                                                       \
        char *dest_c = reinterpret_cast<char *>(dest);                                                                 \
        while (bytes--) {                                                                                              \
            *dest_c = *src_c;                                                                                          \
            ++src_c;                                                                                                   \
            ++dest_c;                                                                                                  \
        }                                                                                                              \
    }                                                                                                                  \
    (__dest, __src, __bytes)
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
#define memmove(__dest, __src, __bytes)                                                                                \
    [](void *dest, const void *src, size_t bytes) -> void *__attribute__((always_inline)) {                            \
        const char *src_c = reinterpret_cast<const char *>(src);                                                       \
        char *dest_c = reinterpret_cast<char *>(dest);                                                                 \
        size_t i = 0;                                                                                                  \
        char *buffer = nullptr;                                                                                        \
        asm("add %0, x0, sp" : "=r"(buffer) : "r"(buffer));                                                            \
        buffer = buffer - bytes;                                                                                       \
        for (; i < bytes; ++i) {                                                                                       \
            buffer[i] = src_c[i];                                                                                      \
            for (; i < bytes; ++i) {                                                                                   \
                dest_c[i] = buffer[i];                                                                                 \
            }                                                                                                          \
        }                                                                                                              \
        return dest;                                                                                                   \
    }                                                                                                                  \
    (__dest, __src, __bytes)
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
#define memset(__src, __c, __sz)                                                                                       \
    [](void *dst, char c, size_t size) __attribute__((always_inline)) {                                                \
        byte *p = reinterpret_cast<byte *>(dst);                                                                       \
        for (size_t i = 0; i < size; ++i) {                                                                            \
            *p = c;                                                                                                    \
        }                                                                                                              \
    }                                                                                                                  \
    (__src, __c, __sz)
#endif

/**
 * @brief Compares **n** bytes at **ptr1** and **ptr2** and returns which is
 * bigger
 *
 * @param ptr1 Pointer to memory region to be compared
 * @param ptr2 Pointer to memory region to be compared
 * @param n Length in bytes to compare
 * @return int 0 if both regions have the same data, -1 if the data at ptr1 <
 * ptr2, 1 if the data at ptr1 > ptr2
 */
#ifndef __MEMCMP
#define __MEMCMP
#define memcmp(__ptr1, __ptr2, __num)                                                                                  \
    [](const void *ptr1, const void *ptr2, size_t num) -> int __attribute__((always_inline)) {                         \
        const byte *p1 = reinterpret_cast<const byte *>(ptr1);                                                         \
        const byte *p2 = reinterpret_cast<const byte *>(ptr2);                                                         \
        for (size_t i = 0; i < num; ++i) {                                                                             \
            byte a = *(p1 + i);                                                                                        \
            byte b = *(p2 + i);                                                                                        \
            if (a < b)                                                                                                 \
                return -1;                                                                                             \
            if (a > b)                                                                                                 \
                return 1;                                                                                              \
        }                                                                                                              \
        return 0;                                                                                                      \
    }                                                                                                                  \
    (__ptr1, __ptr2, __num)
#endif

#ifndef __MEMCHR
#define __MEMCHR
#define memchr(__ptr, __ch, __count)                                                                                   \
    [](const void *ptr, byte ch, size_t count) -> const void *__attribute__((always_inline)) {                         \
        if (ptr != nullptr) {                                                                                          \
            const char *p = reinterpret_cast<const char *>(ptr);                                                       \
            for (size_t i = 0; i < count; ++i) {                                                                       \
                if (*(p + i) == ch)                                                                                    \
                    return (p + i);                                                                                    \
            }                                                                                                          \
        }                                                                                                              \
        return nullptr;                                                                                                \
    }                                                                                                                  \
    (__ptr, __ch, __count)
#endif

/**
 * @brief Converts a pointer value to a unsingned integral
 *
 * @param ptr Pointer to be converted
 * @return uintptr_t Pointer as unsigned integral
 */
#ifndef __TO_UINTPTR
#define __TO_UINTPTR
#define to_uintptr_t(__r)                                                                                              \
    [](const void *p) -> uintptr_t __attribute__((always_inline)) {                                                    \
        return reinterpret_cast<uintptr_t>(p);                                                                         \
    }                                                                                                                  \
    (__r)
#endif

/**
 * @brief Converts a unsigned integral value to a pointer value
 *
 * @param v The value to be converted
 * @return void* Unsigned integral as pointer
 */
#ifndef __TO_PTR
#define __TO_PTR
#define to_ptr(__q)                                                                                                    \
    [](uintptr_t v) -> void *__attribute__((always_inline)) {                                                          \
        return reinterpret_cast<void *>(v);                                                                            \
    }                                                                                                                  \
    (__q)
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
#define apply_offset(__p, __off)                                                                                       \
    [](void *p, uintptr_t offset) -> void *__attribute__((always_inline)) {                                            \
        uintptr_t x = reinterpret_cast<uintptr_t>(p) + offset;                                                         \
        return reinterpret_cast<void *>(x);                                                                            \
    }                                                                                                                  \
    (__p, __off)
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
#define is_aligned(__ptr, __alignment)                                                                                 \
    [](const void *ptr, size_t alignment) -> bool __attribute__((always_inline)) {                                     \
        uintptr_t p = to_uintptr_t(ptr);                                                                               \
        return (p / alignment * alignment) == p;                                                                       \
    }                                                                                                                  \
    (__ptr, __alignment)
#endif

/**
 * @brief Aligns a pointer **ptr** to alignment. If the pointer is not already
 * aligned, it will align it to the next stride, that is, the return value > ptr
 *
 * @param ptr Pointer to be aligned
 * @param alignment The required alignment
 * @return void* The pointer aligned
 */
#ifndef __AFORWARD
#define __AFORWARD
#define align_forward(__ptr, __alignment)                                                                              \
    [](const void *ptr, size_t alignment) -> void *__attribute__((always_inline)) {                                    \
        uintptr_t p = to_uintptr_t(ptr);                                                                               \
        if (alignment == 1 || is_aligned(ptr, alignment))                                                              \
            return to_ptr(p);                                                                                          \
        if (!((alignment - 1) & alignment)) {                                                                          \
            p = (p / alignment) * alignment + alignment;                                                               \
            return to_ptr(p);                                                                                          \
        }                                                                                                              \
        return nullptr;                                                                                                \
    }                                                                                                                  \
    (__ptr, __alignment)
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
#define align_back(__ptr, __alignment)                                                                                 \
    [](const void *ptr, size_t alignment) -> void *__attribute__((always_inline)) {                                    \
        uintptr_t p = to_uintptr_t(ptr);                                                                               \
        if (alignment == 1 || is_aligned(ptr, alignment))                                                              \
            return to_ptr(p);                                                                                          \
        p = (p / alignment) * alignment;                                                                               \
        return to_ptr(p);                                                                                              \
    }                                                                                                                  \
    (__ptr, __alignment)
#endif

#ifndef __BSWAP16
#define __BSWAP16
#define byteswap16(__x)                                                                                                \
    [](uint16_t v) -> uint16_t __attribute__((always_inline)) {                                                        \
        return (v >> 8 | v << 8);                                                                                      \
    }                                                                                                                  \
    (__x)
#endif

#ifndef __BSWAP32
#define __BSWAP32
#define byteswap32(__v)                                                                                                \
    [](uint32_t v) -> uint32_t __attribute__((always_inline)) {                                                        \
        return ((v & 0xFF) << 24) | ((v & 0xFF00) << 8) | ((v >> 8) & 0xFF00) | ((v >> 24) | 0xFF);                    \
    };
#endif

#ifndef __BSWAP64
#define __BSWAP64
#define byteswap64(__v)                                                                                                \
    [](uint64_t v) -> uint64_t __attribute__((always_inline)) {                                                        \
        return v;                                                                                                      \
    }                                                                                                                  \
    (__v)
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
    [](uint16_t data) -> uint16_t __attribute__((always_inline)) {                                                     \
        if constexpr (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) {                                                     \
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
    [](uint32_t data) -> uint32_t __attribute__((always_inline)) {                                                     \
        if constexpr (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) {                                                     \
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
    [](uint64_t data) -> uint64_t __attribute__((always_inline)) {                                                     \
        if constexpr (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) {                                                     \
            return byteswap16(data);                                                                                   \
        }                                                                                                              \
        return data;                                                                                                   \
    }                                                                                                                  \
    (__data)
#endif

} // namespace hls

#endif