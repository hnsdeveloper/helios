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

#ifndef _LIST_HPP_
#define _LIST_HPP_

#include "include/macros.hpp"
#include "ulib/node.hpp"

namespace hls {

template <typename T> class ListNode : Node<T, 1> {
    using nd = Node<T, 1>;
    SET_USING_CLASS(nd, b_node);
    SET_USING_CLASS(ListNode, node);
    EXTRACT_SUB_USING_T_CLASS(nd, type, type);

    using b_node::get_node;
    using b_node::set_node;

  public:
    ListNode(type_const_reference v, node_ptr n) {
        set_data(v);
        set_next(n);
    }

    using b_node::get_data;
    using b_node::set_data;

    void set_next(node_ptr n) {
        nd::template set_node<0>(n);
    }

    node_ptr get_next() {
        const auto &t = *this;
        return const_cast<node_ptr>(t.get_next());
    }

    node_const_ptr get_next() const {
        return reinterpret_cast<node_const_ptr>(nd::template get_node<0>());
    }
};

template <typename T, template <typename> class Allocator> class List {
    using nd = ListNode<T>;
    SET_USING_CLASS(nd, node);
    using _alloc = Allocator<node>;
    SET_USING_CLASS(_alloc, allocator);
    EXTRACT_SUB_USING_T_CLASS(node, type, type);

    size_t m_size = 0;
    node_ptr m_head = nullptr;
    allocator m_allocator;

    template <bool cnst = false> class ListIterator {
        const List *m_list;
        node_const_ptr m_n;

        ListIterator(const List *list, node_const_ptr n) : m_list(list), m_n(n) {};

        template <bool c, typename faketype = void> struct wrapper {
            node_const_ptr n;
            wrapper(node_const_ptr node) : n(node) {};
            node_const_ptr get_n() {
                return n;
            }
        };

        template <typename faketype> struct wrapper<false, faketype> {
            node_const_ptr n;
            wrapper(node_const_ptr node) : n(node) {};
            node_ptr get_n() {
                return const_cast<node_ptr>(n);
            }
        };

        node_ptr get_node() {
            const auto &t = *this;
            return const_cast<node_ptr>(t.get_node());
        }
        node_const_ptr get_node() const {
            return m_n;
        }

        const List *get_list() {
            return m_list;
        }

      public:
        ListIterator(const ListIterator &other) : m_list(other.m_list), m_n(other.m_n) {};

        auto &operator*() const {
            auto n = wrapper<cnst>(m_n).get_n();
            return n->get_data();
        }

        auto operator->() const {
            auto n = wrapper<cnst>(m_n).get_n();
            return &(n->get_data());
        }

        ListIterator &operator++() {
            m_n = m_n->get_next();
            return *this;
        }

        ListIterator operator++(int) {
            auto other = *this;
            ++(*this);
            return other;
        }

        ListIterator &operator=(const ListIterator &other) {
            m_list = other.m_list;
            m_n = other.m_n;
            return *this;
        }

        bool operator==(const ListIterator &other) const {
            return m_n == other.m_n && m_list == other.m_list;
        }

        bool operator!=(const ListIterator &other) const {
            return !(*(this) == other);
        }

        friend class List;
    };

    using it = ListIterator<false>;
    using cit = ListIterator<true>;

    SET_USING_CLASS(it, iterator);
    SET_USING_CLASS(cit, const_iterator);

  public:
    List() {
        m_head = nullptr;
    }

    ~List() {
        clear();
    }

    iterator insert_after(iterator at, type_const_reference v) {
        if (at.get_list() != this)
            return end();

        if (at == end() && size() > 0)
            return end();

        if (size() == max_size())
            return end();

        node_ptr n = m_allocator.create(v, nullptr);
        if (m_head != nullptr) {
            node_ptr m = const_cast<node_ptr>(at.get_node());
            node_ptr next = m->get_next();
            m->set_next(n);
            n->set_next(next);
        } else {
            m_head = n;
        }

        ++m_size;

        return iterator(this, n);
    }

    void erase_after(iterator at) {
        if (at == end())
            return;
        if (at.get_list() != this)
            return;

        node_ptr n = at.get_node();

        if (n->get_next() != nullptr) {
            node_ptr d = n->get_next();
            n->set_next(d->get_next());
            m_allocator.destroy(d);
            --m_size;
        }
    }

    iterator push_front(type_const_reference v) {
        if (size() == max_size())
            return end();
        node_ptr n = m_allocator.create(v, m_head);
        n->set_next(m_head);
        m_head = n;
        ++m_size;
        return iterator(this, m_head);
    }

    iterator pop_front() {
        if (m_head) {
            node_ptr n = m_head;
            m_head = n->get_next();
            m_allocator.destroy(n);
            --m_size;
        }

        return begin();
    }

    iterator begin() {
        return iterator(this, m_head);
    }

    iterator end() {
        return iterator(this, nullptr);
    }

    const_iterator begin() const {
        return const_iterator(this, m_head);
    }

    const_iterator end() const {
        return const_iterator(this, nullptr);
    }

    void clear() {
        if (m_head) {
            node_ptr n = m_head;
            while (m_head) {
                m_head = n->get_next();
                m_allocator.destroy(n);
                n = m_head;
            }
        }
    }

    size_t size() const {
        return m_size;
    }

    size_t max_size() const {
        return __UINT64_MAX__;
    }

    void resize(size_t n) {
        size_t c = 0;

        node_ptr p = m_head;
        while (p) {
            p = p->get_next();
            ++c;
            if (c == n)
                break;
        }

        if (c == n) {
            node_ptr to_remove = p->get_next();
            p->set_next(nullptr);

            while (to_remove) {
                node_ptr tmp = to_remove;
                to_remove = to_remove->get_next();
                m_allocator.destroy(tmp);
            }
        }

        if (c < n) {
            if (c == 0) {
                m_head = m_allocator.create();
                p = m_head;
                ++c;
            }

            while (c != n) {
                p->set_next(m_allocator.create());
                p = p->get_next();
                ++c;
            }
        }
    }
};

} // namespace hls

#endif