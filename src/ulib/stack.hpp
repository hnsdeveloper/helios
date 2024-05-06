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

#ifndef _STACK_HPP_
#define _STACK_HPP_

#include "include/macros.hpp"
#include "ulib/double_list.hpp"

namespace hls {

template <typename T, template <class, class> class ds = DoubleList, template <class> class Allocator> class Stack {
    SET_USING_CLASS(T, type);
    using _underlying = ds<type, Allocator>;
    SET_USING_CLASS(_underlying, Und);
    EXTRACT_SUB_USING_T_CLASS(Und, allocator, allocator);

  public:
    using ds::empty;
    using ds::pop_back;
    using ds::size;

    void push_back(type_const_reference v) {
        ds::push_back(v);
    }

    type_reference top() {
        const auto &as_const = *this;
        return const_cast<type_reference>(as_const.top());
    }

    type_const_reference top() {
        if (empty())
            PANIC("Requesting element from empty stack.");
        return *(--ds::end());
    }
};

} // namespace hls

#endif