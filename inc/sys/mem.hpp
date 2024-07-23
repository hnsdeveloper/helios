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

#include "arch/riscv64/plat_def.hpp"
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
    void *memcpy(void *dest, const void *src, size_t bytes);

    /**
     * @brief Copies **bytes** from src to dest, using a buffer in case regions
     * overlap
     *
     * @param dest Pointer to where data will be copied to
     * @param src Source data pointer
     * @param bytes The size of the data to be moved
     * @return void* pointer to dest
     */
    void *memmove(void *dest, const void *src, size_t bytes);

    /**
     * @brief Sets **n** bytes at **dest** to **c**
     *
     * @param dst Destination pointer.
     * @param c The value memory will be set to
     * @param size Length of memory to be set
     */
    void memset(void *dst, char c, size_t size);

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
    int memcmp(const void *ptr1, const void *ptr2, size_t num);

    void *memchr(const void *ptr, byte ch, size_t count);

    /**
     * @brief Converts a pointer value to a unsingned integral
     *
     * @param p Pointer to be converted
     * @return uintptr_t Pointer as unsigned integral
     */
    uintptr_t to_uintptr_t(const void *p);

    /**
     * @brief Converts a unsigned integral value to a pointer value
     *
     * @param v The value to be converted
     * @return void* Unsigned integral as pointer
     */
    void *to_ptr(uintptr_t v);

    /**
     * @brief Converts any pointer type to a byte* type.
     *
     * @param p The pointer to be converted
     * @return byte* Pointer as byte* type
     */
    byte *as_byte_ptr(const void *p);

    /**
     * @brief Applies an offset (in bytes) to a pointer value
     *
     * @param p The pointer to apply an offset to
     * @param offset Offset in bytes
     * @return const void* Pointer with offset applied
     */
    void *apply_offset(void *p, uintptr_t offset);

    /**
     * @brief Checks if a pointer follows alignment **alignment**
     *
     * @param ptr Pointer value to check alignment
     * @param alignment Alignment value
     * @return true If the pointer is aligned
     * @return false If the pointer is not aligned
     */
    bool is_aligned(const void *ptr, size_t alignment);

    /**
     * @brief Returns the absolute difference (in bytes) between two pointers.
     *
     * @param a One of the pointers.
     * @param b The other pointer.
     * @return size_t The absolute difference between both pointers.
     */
    size_t abs_ptr_diff(const void *a, const void *b);

    /**
     * @brief Aligns a pointer **ptr** to alignment. If the pointer is not already
     * aligned, it will align it to the next stride, that is, the return value > ptr
     *
     * @param ptr Pointer to be aligned
     * @param alignment The required alignment
     * @return void* The pointer aligned
     */
    void *align_forward(const void *ptr, size_t alignment);

    /**
     * @brief Aligns a pointer **ptr** to alignment. If the pointer is not already
     * aligned, it will align it to the stride before, that is, the return value <
     * ptr
     *
     * @param ptr Pointer to be aligned
     * @param alignment The required alignment
     * @return void* The pointer aligned
     */
    void *align_back(const void *ptr, size_t alignment);

    uint16_t byteswap16(uint16_t v);

    uint32_t byteswap32(uint32_t v);

    uint64_t byteswap64(uint64_t v);

    /**
     * @brief Reads a big endian uint16_t value
     *
     * @param data The value to be read
     * @return uint16_t A native endian uint16_t
     */
    uint16_t uint16_read_be(uint16_t data);

    /**
     * @brief Reads a big endian uint32_t value
     *
     * @param data The value to be read
     * @return uint16_t A native endian uint32_t
     */
    uint32_t uint32_read_be(uint32_t data);

    /**
     * @brief Reads a big endian uint64_t value
     *
     * @param data The value to be read
     * @return uint16_t A native endian uint64_t
     */
    uint64_t uint64_read_be(uint64_t data);

    size_t get_needed_pages(const void *mem, size_t size, FrameOrder lvl);

    bool is_power_of_two(size_t val);
} // namespace hls

#endif
