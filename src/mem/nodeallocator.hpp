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

#ifndef _NODEALLOCATOR_HPP_
#define _NODEALLOCATOR_HPP_

#include "arch/riscv64gc/plat_def.hpp"
#include "mem/bumpallocator.hpp"
#include "misc/types.hpp"
#include "misc/utilities.hpp"
#include "sys/mem.hpp"

namespace hls
{
    template <typename T>
    class NodeAllocator
    {
        SET_USING_CLASS(T, type);
        BumpAllocator &m_bump_allocator;

      public:
        NodeAllocator(BumpAllocator &allocator) : m_bump_allocator(allocator)
        {
        }

        template <typename... Args>
        type_ptr create(Args... args)
        {
            type_ptr v = allocate();
            if (v != nullptr)
            {
                new (v) type(hls::forward<Args>(args)...);
            }
            return v;
        }

        void destroy(type_const_ptr p)
        {
            if (p == nullptr)
                return;

            type_ptr p_nc = const_cast<type_ptr>(p);
            (*p_nc).~type();
            deallocate(p_nc);
        }

        type_ptr allocate()
        {
            void *p = m_bump_allocator.get_mem();
            return reinterpret_cast<type_ptr>(p);
        }

        void deallocate(type_const_ptr p)
        {
            if (p == nullptr)
                return;
            m_bump_allocator.release_mem(p);
        }
    };

} // namespace hls

#endif
