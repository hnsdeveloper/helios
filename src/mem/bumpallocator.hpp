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

#include "arch/riscv64gc/plat_def.hpp"
#include "misc/types.hpp"
#include "sys/mem.hpp"

namespace hls
{
    class BumpAllocator
    {
        byte *m_initialmemory[FrameKB::s_size];
        void *m_items_list;
        size_t m_items_count;
        size_t m_type_size;

      public:
        BumpAllocator(size_t typesize) : m_items_list(nullptr), m_items_count(0), m_type_size(typesize)
        {
            expand_from_frame(m_initialmemory);
        }

        void expand_from_frame(void *frame_address)
        {
            for (byte *i = as_byte_ptr(frame_address); i < as_byte_ptr(frame_address) + FrameKB::s_size;
                 i += m_type_size)
                release_mem(i);
        }

        void *get_mem()
        {
            if (m_items_count)
            {
                m_items_count = m_items_count - 1;
                auto temp = m_items_list;
                uintptr_t &p = *reinterpret_cast<uintptr_t *>(m_items_list);
                m_items_list = to_ptr(p);
                return temp;
            }
            return nullptr;
        }

        void release_mem(const void *ptr)
        {
            uintptr_t &v = const_cast<uintptr_t &>(*reinterpret_cast<const uintptr_t *>(ptr));
            v = to_uintptr_t(m_items_list);
            m_items_list = const_cast<void *>(ptr);
            m_items_count = m_items_count + 1;
        }

        size_t available_count() const
        {
            return m_items_count;
        }
    };
} // namespace hls

#endif /* bumpallocator_hpp */
