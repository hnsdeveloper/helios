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

#ifndef _BUMPALLOCATOR_HPP_
#define _BUMPALLOCATOR_HPP_

#include "arch/riscv64/plat_def.hpp"
#include "klibc/kstddef.h"
#include "klibc/kstdint.h"

namespace hls
{
    class BumpAllocator
    {
        byte m_initialmemory[FrameKB::s_size];
        void *m_items_list;
        size_t m_items_count;
        size_t m_type_size;
        size_t m_min_items_threshold;

      public:
        BumpAllocator(size_t typesize);
        BumpAllocator(const BumpAllocator &) = delete;
        BumpAllocator(BumpAllocator &&) = delete;

        void expand_from_frame(void *frame_address);
        void *get_mem();

        void release_mem(const void *ptr);

        size_t available_count() const;
    };
} // namespace hls

#endif /* bumpallocator_hpp */
