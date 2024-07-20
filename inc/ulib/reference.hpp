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

#ifndef _REFERENCE_HPP_
#define _REFERENCE_HPP_

#include "misc/macros.hpp"

namespace hls
{
    template <typename T>
    class Ref
    {
        SET_USING_CLASS(T, value);

        value_ptr m_value;

      public:
        Ref(value_reference v)
        {
            m_value = &v;
        }
        Ref(const Ref &other)
        {
            m_value = other.m_value;
        }
        ~Ref() = default;

        operator value_reference()
        {
            const auto &as_const = *this;
            value_const_reference v = as_const;
            return const_cast<value_reference>(v);
        }

        operator value_const_reference() const
        {
            return *m_value();
        }

        value_reference get()
        {
            const auto &c = *this;
            return const_cast<value_reference>(c.get());
        }

        value_const_reference get() const
        {
            return *m_value;
        }
    };

    template <typename T>
    Ref<T> make_ref(T &v)
    {
        return Ref<T>(v);
    }

}; // namespace hls

#endif