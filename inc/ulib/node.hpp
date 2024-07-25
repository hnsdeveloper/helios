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

#ifndef _NODE_HPP_
#define _NODE_HPP_

#include "klibc/kstddef.h"
#include "klibc/kstdint.h"
#include "misc/macros.hpp"
#include "misc/utilities.hpp"

namespace hls
{

    template <typename Data, size_t N>
    class Node : protected Node<Data, N - 1>
    {
        using nd = Node<Data, N - 1>;
        EXTRACT_SUB_USING_T_CLASS(nd, type, type);
        SET_USING_CLASS(Node, node);
        void *m_p;

      public:
        Node() = default;
        ~Node() = default;
        Node(node_const_reference other) = delete;

        using nd::get_data;
        using nd::set_data;

        template <size_t M>
        node_ptr get_node()
        {
            const auto &t = *this;
            const void *p = t.template get_node<M>();
            return reinterpret_cast<node_ptr>(const_cast<void *>(p));
        }

        template <size_t M>
        node_const_ptr get_node() const
        {
            static_assert(M <= N, "Invalid M template argument. Should be smaller than N.");
            if constexpr (M == N - 1)
            {
                return reinterpret_cast<node_const_ptr>(m_p);
            }
            else if constexpr (true)
            {
                return reinterpret_cast<node_const_ptr>(nd::template get_node<M>());
            }
        }

        template <size_t M>
        void set_node(node_ptr n)
        {
            static_assert(M <= N, "Invalid M template argument. Should be smaller than N.");
            if constexpr (M == N - 1)
            {
                m_p = n;
            }
            else if constexpr (true)
                nd::template set_node<M>(n);
        }
    };

    template <typename T>
    class Node<T, 1>
    {
        SET_USING_CLASS(T, type);
        SET_USING_CLASS(Node, node);

        const void *m_p;
        char m_data[sizeof(type)];
        bool m_initialized;

      public:
        Node() = default;
        ~Node()
        {
            if (m_initialized)
            {
                type_ptr p = reinterpret_cast<type_ptr>(m_data);
                (*p).~type();
            }
        }

        void set_data(type_const_reference v)
        {
            type_ptr p = reinterpret_cast<type_ptr>(m_data);
            new (p) type(v);
            m_initialized = true;
        }

        void set_data(type_rvalue_reference v)
        {
            type_ptr p = reinterpret_cast<type_ptr>(m_data);
            new (p) type(hls::move(v));
            m_initialized = true;
        }

        type_reference get_data()
        {
            const auto &t = *this;
            return const_cast<type_reference>(t.get_data());
        }

        type_const_reference get_data() const
        {
            return *reinterpret_cast<type_const_ptr>(m_data);
        }

        template <size_t M>
        node_ptr get_node()
        {
            static_assert(M == 0, "M is invalid, should be 0.");
            if constexpr (M == 0)
                return reinterpret_cast<node_ptr>(m_p);
        }

        template <size_t M>
        node_const_ptr get_node() const
        {
            static_assert(M == 0, "M is invalid, should be 0.");
            if constexpr (M == 0)
                return reinterpret_cast<node_const_ptr>(m_p);
        }

        template <size_t M>
        void set_node(node_const_ptr n)
        {
            static_assert(M == 0, "M is invalid, should be 0.");
            if constexpr (M == 0)
                m_p = n;
        }
    };

    template <typename T>
    class Node<T, 0>;

} // namespace hls

#endif