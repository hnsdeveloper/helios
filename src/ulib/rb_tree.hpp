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

#ifndef _RB_TREE_HPP_
#define _RB_TREE_HPP_

#include "misc/limits.hpp"
#include "misc/macros.hpp"
#include "misc/types.hpp"
#include "ulib/hash.hpp"
#include "ulib/node.hpp"

namespace hls
{

    enum Color
    {
        RED,
        BLACK
    };

    template <typename T>
    class RBTreeNode : protected Node<T, 4>
    {
        using rbtn = RBTreeNode;
        using nd = Node<T, 4>;
        SET_USING_CLASS(rbtn, node);
        EXTRACT_SUB_USING_T_CLASS(nd, type, type);

        Color m_color;

        // Regarding nodes, each index corresponds to a "position"
        // 0 = parent
        // 1 = left
        // 2 = right
        // 3 = t_null

      public:
        RBTreeNode(type data, Color color, node_ptr t_null) : m_color(color)
        {
            nd::set_data(hls::move(data));
            nd::template set_node<0>(t_null);
            nd::template set_node<1>(t_null);
            nd::template set_node<2>(t_null);
            nd::template set_node<3>(t_null);
        };

        RBTreeNode()
        {
            nd::template set_node<3>(this);
            set_color(Color::BLACK);
        }

        RBTreeNode(const RBTreeNode &) = delete;
        RBTreeNode(RBTreeNode &&) = delete;

        using nd::get_data;
        using nd::set_data;

        void set_parent(node_ptr n)
        {
            nd::template set_node<0>(n);
        }

        void set_left(node_ptr n)
        {
            nd::template set_node<1>(n);
        }

        void set_right(node_ptr n)
        {
            nd::template set_node<2>(n);
        }

        node_ptr get_parent()
        {
            const auto &t = *this;
            return const_cast<node_ptr>(t.get_parent());
        }

        node_ptr get_left()
        {
            const auto &t = *this;
            return const_cast<node_ptr>(t.get_left());
        }

        node_ptr get_right()
        {
            const auto &t = *this;
            return const_cast<node_ptr>(t.get_right());
        }

        node_const_ptr get_parent() const
        {
            return reinterpret_cast<node_const_ptr>(nd::template get_node<0>());
        }

        node_const_ptr get_left() const
        {
            return reinterpret_cast<node_const_ptr>(nd::template get_node<1>());
        }

        node_const_ptr get_right() const
        {
            return reinterpret_cast<node_const_ptr>(nd::template get_node<2>());
        }

        void set_color(Color n)
        {
            m_color = n;
        }

        Color get_color() const
        {
            return m_color;
        }

        node_const_ptr null() const
        {
            return reinterpret_cast<node_const_ptr>(nd::template get_node<3>());
        }
    };

    template <typename T>
    class LessComparator
    {
        SET_USING_CLASS(T, type);

      public:
        bool operator()(type_const_reference a, type_const_reference b) const
        {
            return a < b;
        }
    };

    template <typename T, template <typename> class Hash, template <typename> class Cmp,
              template <typename> class Alloc, bool dup = false>
    class RedBlackTree
    {
        EXTRACT_SUB_USING_T_CLASS(RBTreeNode<T>, node, node);
        EXTRACT_SUB_USING_T_CLASS(node, type, type);
        using _hs = Hash<type>;
        SET_USING_CLASS(_hs, hsh);
        EXTRACT_SUB_USING_T_CLASS(hsh, type, hashed);
        EXTRACT_SUB_USING_T_CLASS(hsh, hash_result, hash_result);

        using _cmp = Cmp<hash_result>;
        SET_USING_CLASS(_cmp, comparator);

        using _alloc = Alloc<node>;
        SET_USING_CLASS(_alloc, allocator);

        template <bool reverse, bool cnst = false>
        class RBTreeIterator
        {
            const RedBlackTree *m_tree;
            node_const_ptr m_n;

            RBTreeIterator(const RedBlackTree *tree, node_const_ptr n) : m_tree(tree), m_n(n) {};

            template <bool c, typename fake = void>
            struct wrapper
            {
                node_const_ptr n;
                wrapper(node_const_ptr node) : n(node) {};
                node_const_ptr get_n()
                {
                    return n;
                }
            };

            template <typename fake>
            struct wrapper<false, fake>
            {
                node_const_ptr n;
                wrapper(node_const_ptr node) : n(node) {};
                node_ptr get_n()
                {
                    return const_cast<node_ptr>(n);
                }
            };

          public:
            RBTreeIterator(const RBTreeIterator &other) : m_tree(other.m_tree), m_n(other.m_n) {};

            auto &operator*() const
            {
                auto n = wrapper<cnst>(m_n).get_n();
                return n->get_data();
            }

            auto operator->() const
            {
                auto n = wrapper<cnst>(m_n).get_n();
                return &(n->get_data());
            }

            RBTreeIterator &operator++()
            {
                if constexpr (reverse == false)
                {
                    m_n = m_tree->get_in_order_successor(m_n);
                }
                if constexpr (reverse == true)
                {
                    m_n = m_tree->get_in_order_predecessor(m_n);
                }
                return *this;
            }

            RBTreeIterator operator++(int)
            {
                auto other = *this;
                ++(*this);
                return other;
            }

            RBTreeIterator &operator--()
            {
                if constexpr (reverse == false)
                {
                    m_n = m_tree->get_in_order_predecessor(m_n);
                }
                if constexpr (reverse == true)
                {
                    m_n = m_tree->get_in_order_successor(m_n);
                }
                return *this;
            }

            RBTreeIterator operator--(int)
            {
                auto other = *this;
                --(*this);
                return other;
            }

            RBTreeIterator &operator=(const RBTreeIterator &other)
            {
                m_tree = other.m_tree;
                m_n = other.m_n;

                return *this;
            }

            bool operator==(const RBTreeIterator &other) const
            {
                return m_n == other.m_n && m_tree == other.m_tree;
            }

            bool operator!=(const RBTreeIterator &other) const
            {
                return !(*(this) == other);
            }

            operator RBTreeIterator<reverse, true>() const
            {
                return RBTreeIterator<reverse, true>(m_tree, m_n);
            }

            friend class RedBlackTree;
        };

        using it = RBTreeIterator<false>;
        using rit = RBTreeIterator<true>;
        using cit = RBTreeIterator<false, true>;
        using crit = RBTreeIterator<true, true>;

        SET_USING_CLASS(it, iterator);
        SET_USING_CLASS(rit, reverse_iterator);
        SET_USING_CLASS(cit, const_iterator);
        SET_USING_CLASS(crit, const_reverse_iterator);

        hsh m_hasher;
        node m_t_null;
        node_ptr m_root;
        size_t m_size;
        allocator m_allocator;
        comparator m_comparator;

        void move_helper(RedBlackTree &other, node_ptr n)
        {
            if (n->get_left() == other.null())
                n->set_left(null());
            else
                move_helper(other, n->get_left());

            if (n->get_right() == other.null())
                n->set_right(null());
            else
                move_helper(other, n->get_right());

            if (n == m_root)
            {
                if (n == other.null())
                    m_root = null();
                else
                    n->set_parent(nullptr);
            }
        }

        node_const_ptr find_helper(hash_result_const_reference key, node_const_ptr *parent_save) const
        {
            comparator_const c = get_comparator();
            hsh_const h = get_hasher();
            node_ptr current = m_root;
            *parent_save = null();
            while (current != null())
            {
                if (c(key, h(current->get_data())))
                {
                    *parent_save = current;
                    current = current->get_left();
                    continue;
                }
                if (c(h(current->get_data()), key))
                {
                    *parent_save = current;
                    current = current->get_right();
                    continue;
                }

                break;
            }
            return current;
        }

        node_ptr find_helper(hash_result_const_reference key, node_ptr *p)
        {
            const auto &c = *this;
            node_const_ptr r = nullptr;
            node_const_ptr p2 = nullptr;

            r = c.find_helper(key, &p2);
            *p = const_cast<node_ptr>(p2);
            return const_cast<node_ptr>(r);
        }

        void insert_fix(node_ptr n)
        {
            node_ptr p = nullptr;
            node_ptr u = nullptr;
            node_ptr gp = nullptr;

            if (n == m_root)
            {
                n->set_color(Color::BLACK);
                return;
            }

            p = n->get_parent();

            if (!is_red(n) || !is_red(p))
            {
                return;
            }
            if (p)
            {
                gp = p->get_parent();
            }
            if (gp)
            {
                u = gp->get_left() == p ? gp->get_right() : gp->get_left();
            }

            if (is_red(u))
            {
                gp->set_color(Color::RED);
                u->set_color(Color::BLACK);
                p->set_color(Color::BLACK);
                insert_fix(gp);
            }
            else if (is_black(u))
            {
                if (is_right_child(p))
                {
                    if (is_right_child(n))
                    {
                        rotate_left(gp);
                        p->set_color(Color::BLACK);
                        gp->set_color(Color::RED);
                    }
                    else if (is_left_child(n))
                    {
                        n->set_color(Color::BLACK);
                        p->set_color(Color::RED);
                        gp->set_color(Color::RED);
                        rotate_right(p);
                        rotate_left(gp);
                    }
                }
                else if (is_left_child(p))
                {
                    if (is_left_child(n))
                    {
                        rotate_right(gp);
                        p->set_color(Color::BLACK);
                        gp->set_color(Color::RED);
                    }
                    else if (is_right_child(n))
                    {
                        n->set_color(Color::BLACK);
                        p->set_color(Color::RED);
                        gp->set_color(Color::RED);
                        rotate_left(p);
                        rotate_right(gp);
                    }
                }
            }
        }

        void remove_fix(node_ptr n)
        {
            while (n != m_root && is_black(n))
            {
                node_ptr p = n->get_parent();
                node_ptr s;
                if (is_left_child(n, p))
                {
                    s = p->get_right();
                    if (is_red(s))
                    {
                        s->set_color(Color::BLACK);
                        p->set_color(Color::RED);
                        rotate_left(p);
                        s = p->get_right();
                    }
                    if (is_black(s->get_left()) && is_black(s->get_right()))
                    {
                        s->set_color(Color::RED);
                        n = p;
                        p = p->get_parent();
                    }
                    else
                    {
                        if (is_black(s->get_right()))
                        {
                            s->get_left()->set_color(Color::BLACK);
                            s->set_color(Color::RED);
                            rotate_right(s);
                            s = p->get_right();
                        }
                        s->set_color(p->get_color());
                        p->set_color(Color::BLACK);
                        s->get_right()->set_color(Color::BLACK);
                        rotate_left(p);
                        n = m_root;
                    }
                }
                else
                {
                    s = p->get_left();
                    if (is_red(s))
                    {
                        s->set_color(Color::BLACK);
                        p->set_color(Color::RED);
                        rotate_right(p);
                        s = p->get_left();
                    }
                    if (is_black(s->get_right()) && is_black(s->get_left()))
                    {
                        s->set_color(Color::RED);
                        n = p;
                        p = p->get_parent();
                    }
                    else
                    {
                        if (is_black(s->get_left()))
                        {
                            s->get_right()->set_color(Color::BLACK);
                            s->set_color(Color::RED);
                            rotate_left(s);
                            s = p->get_left();
                        }
                        s->set_color(p->get_color());
                        p->set_color(Color::BLACK);
                        s->get_left()->set_color(Color::BLACK);
                        rotate_right(p);
                        n = m_root;
                    }
                }
            }
            if (n)
                n->set_color(Color::BLACK);
        }

        bool is_left_child(node_const_ptr n, node_const_ptr p = nullptr) const
        {
            if (n != nullptr)
            {
                if (n->get_parent() != nullptr)
                {
                    return n->get_parent()->get_left() == n;
                }
            }
            if (p != nullptr)
            {
                if (p->get_left() == n)
                    return true;
            }
            return false;
        }

        bool is_right_child(node_const_ptr n, node_const_ptr p = nullptr) const
        {
            if (n)
            {
                if (n->get_parent())
                {
                    return n->get_parent()->get_right() == n;
                }
            }
            if (p)
            {
                if (n == p->get_right())
                    return true;
            }
            return false;
        }

        void rotate_left(node_ptr n)
        {
            auto parent = n->get_parent();
            auto newparent = n->get_right();
            auto new_right = n->get_right()->get_left();
            n->set_parent(newparent);
            n->set_right(new_right);
            new_right->set_parent(n);
            newparent->set_left(n);
            newparent->set_parent(parent);

            if (parent != nullptr)
            {
                if (n == parent->get_left())
                    parent->set_left(newparent);
                else
                    parent->set_right(newparent);
            }
            else
                m_root = newparent;
        }

        void rotate_right(node_ptr n)
        {
            auto parent = n->get_parent();
            auto newparent = n->get_left();
            auto new_left = n->get_left()->get_right();

            n->set_parent(newparent);
            n->set_left(new_left);
            new_left->set_parent(n);
            newparent->set_right(n);
            newparent->set_parent(parent);

            if (parent != nullptr)
            {
                if (n == parent->get_left())
                    parent->set_left(newparent);
                else
                    parent->set_right(newparent);
            }
            else
                m_root = newparent;
        }

        inline bool is_red(node_ptr p)
        {
            if (p)
                return p->get_color() == Color::RED;
            return false;
        }

        inline bool is_black(node_ptr p)
        {
            return !is_red(p);
        }

        node_ptr find_minimum(node_ptr n)
        {
            const auto &c = *this;
            return const_cast<node_ptr>(c.find_minimum(n));
        }

        node_const_ptr find_minimum(node_const_ptr n) const
        {
            node_const_ptr ret_val = n;
            while (n != null())
            {
                ret_val = n;
                n = n->get_left();
            }
            return ret_val;
        }

        node_ptr find_maximum(node_ptr n)
        {
            const auto &c = *this;
            return const_cast<node_ptr>(c.find_maximum(n));
        }

        node_const_ptr find_maximum(node_const_ptr n) const
        {
            node_const_ptr ret_val = n;
            while (n != null())
            {
                ret_val = n;
                n = n->get_right();
            }
            return ret_val;
        }

        void transplant(node_ptr n, node_ptr sn)
        {
            if (n == null())
                return;

            node_ptr p = n->get_parent();
            if (p != nullptr)
            {
                if (p->get_left() == n)
                    p->set_left(sn);
                if (p->get_right() == n)
                    p->set_right(sn);
            }
            else
            {
                m_root = sn;
            }

            if (sn)
            {
                if (sn != n->get_left())
                    sn->set_left(n->get_left());
                if (sn != n->get_right())
                    sn->set_right(n->get_right());
                if (sn->get_left())
                    sn->get_left()->set_parent(sn);
                if (sn->get_right())
                    sn->get_right()->set_parent(sn);
                sn->set_parent(n->get_parent());
            }
        }

        void destructor_helper(node_ptr n)
        {
            if (n == null())
                return;

            if (n->get_left() != null())
            {
                destructor_helper(n->get_left());
                m_allocator.destroy(n->get_left());
            }
            if (n->get_right() != null())
            {
                destructor_helper(n->get_right());
                m_allocator.destroy(n->get_right());
            }

            if (n == m_root)
                m_allocator.destroy(n);
        }

      public:
        RedBlackTree()
        {
            m_root = null();
            m_size = 0;
        }

        ~RedBlackTree()
        {
            if (m_root != null())
                destructor_helper(m_root);
        }

        RedBlackTree(const RedBlackTree &other) : RedBlackTree()
        {
            *this = other;
        }

        RedBlackTree(RedBlackTree &&other) : RedBlackTree()
        {
            *this = hls::move(other);
        }

        void remove(type_const_reference key)
        {
            remove(get_hasher()(key));
        }

        void remove(hash_result_const_reference hash)
        {
            node_ptr p = nullptr;
            node_ptr n = find_helper(hash, &p);
            node_ptr x = null();
            if (n == null())
                return;
            p = n->get_parent();
            Color original_color = n->get_color();

            if (n->get_left() == null())
            {
                x = n->get_right();
                transplant(n, x);
            }
            else if (n->get_right() == null())
            {
                x = n->get_left();
                transplant(n, x);
            }
            else
            {
                node_ptr y = find_minimum(n->get_right());
                original_color = y->get_color();
                x = y->get_right();
                if (y == n->get_right())
                {
                    x->set_parent(y);
                }
                else
                {
                    transplant(y, x);
                    x->set_parent(y->get_parent());
                }
                transplant(n, y);

                y->set_color(original_color);
            }

            if (original_color == Color::BLACK)
                remove_fix(x);

            m_root->set_parent(nullptr);

            m_allocator.destroy(n);
            --m_size;
        }

        node_ptr insert(type_const_reference key)
        {
            if (size() == max_size())
                return null();

            auto &c = get_comparator();
            auto &h = get_hasher();

            node_ptr n = m_allocator.create(key, Color::RED, null());

            if (m_root != null())
            {
                // Find to which node we are going to insert node n
                node_ptr p;
                find_helper(h(key), &p);
                if (c(h(key), h(p->get_data())))
                {
                    p->set_left(n);
                }
                else
                {
                    p->set_right(n);
                }

                n->set_parent(p);
                insert_fix(n);
             }
            else
            {
                m_root = n;
                n->set_color(Color::BLACK);
            }

            m_root->set_parent(nullptr);
            ++m_size;
            return n;
        }

        bool contains(type_const_reference key) const
        {
            return contains(get_hasher()(key));
        }

        bool contains(hash_result_const_reference hash) const
        {
            node_const_ptr p;
            return find_helper(hash, &p) != null();
        }

        node_ptr get_node(hash_result_const_reference hash)
        {
            const auto &c = *this;
            return const_cast<node_ptr>(c.get_node(hash));
        }

        node_const_ptr get_node(hash_result_const_reference hash) const
        {
            node_const_ptr p = nullptr;
            return find_helper(hash, &p);
        }

        node_ptr get_node(type_const_reference key)
        {
            const auto &c = *this;
            return const_cast<node_ptr>(c.get_node(key));
        }

        node_const_ptr get_node(type_const_reference key) const
        {
            return get_node(get_hasher()(key));
        }

        node_ptr get_in_order_successor(node_ptr n)
        {
            const auto &c = *this;
            return const_cast<node_ptr>(c.get_in_order_successor(n));
        }

        node_const_ptr get_in_order_successor(node_const_ptr n) const
        {
            if (m_root == null())
                return null();

            if (n == nullptr)
            {
                return find_minimum(m_root);
            }

            if (n != null())
            {
                if (n->get_right() != null())
                {
                    return find_minimum(n->get_right());
                }
                else
                {
                    while (n != nullptr && !is_left_child(n))
                    {
                        n = n->get_parent();
                    }
                    if (n != nullptr)
                        return n->get_parent();
                }
            }

            return null();
        }

        node_ptr get_in_order_predecessor(node_ptr n)
        {
            const auto &c = *this;
            return const_cast<node_ptr>(c.get_in_order_predecessor(n));
        }

        node_const_ptr get_in_order_predecessor(node_const_ptr n) const
        {
            if (m_root == null())
                return nullptr;

            if (n != nullptr)
            {
                if (n == null())
                    return find_maximum(m_root);
                if (n->get_left() != null())
                    return find_maximum(n->get_left());
                if (is_right_child(n))
                    return n->get_parent();
                while (n != nullptr && !is_right_child(n))
                {
                    n = n->get_parent();
                }

                if (n != nullptr)
                    return n->get_parent();
            }

            return nullptr;
        }

        node_const_ptr get_root() const
        {
            return m_root;
        }

        node_ptr null()
        {
            const auto &c = *this;
            return const_cast<node_ptr>(c.null());
        }

        node_const_ptr null() const
        {
            return &m_t_null;
        }

        node_ptr equal_or_greater(type_const_reference key)
        {
            const auto &c = *this;
            return const_cast<node_ptr>(c.equal_or_greater(key));
        }

        node_const_ptr equal_or_greater(type_const_reference key) const
        {
            return equal_or_greater(get_hasher()(key));
        }

        node_ptr equal_or_greater(hash_result_const_reference hash)
        {
            const auto &c = *this;
            return const_cast<node_ptr>(c.equal_or_greater(hash));
        }

        node_const_ptr equal_or_greater(hash_result_const_reference k) const
        {
            node_const_ptr p = nullptr;
            node_const_ptr n = find_helper(k, &p);
            return null() != n ? n : p;
        }

        iterator begin()
        {
            return iterator(this, get_in_order_successor(nullptr));
        }

        iterator end()
        {
            return iterator(this, null());
        }

        reverse_iterator rbegin()
        {
            return reverse_iterator(this, get_in_order_predecessor(null()));
        }

        reverse_iterator rend()
        {
            return reverse_iterator(this, nullptr);
        }

        const_iterator begin() const
        {
            return const_iterator(this, get_in_order_successor(nullptr));
        }

        const_iterator end() const
        {
            return const_iterator(this, null());
        }

        const_reverse_iterator rbegin() const
        {
            return const_reverse_iterator(this, get_in_order_predecessor(null()));
        }

        const_reverse_iterator rend() const
        {
            return const_reverse_iterator(this, nullptr);
        }

        bool empty() const
        {
            return m_size == 0;
        }

        size_t size() const
        {
            return m_size;
        }

        size_t max_size() const
        {
            return hls::limit<size_t>::max;
        }

        template <bool a, bool b>
        bool is_tree_iterator(const RBTreeIterator<a, b> &it)
        {
            return it.m_tree == this;
        }

        template <typename IteratorType>
        IteratorType build_iterator(node_const_ptr n) const
        {
            if (n)
            {
                if (n->null() == null())
                    return IteratorType(this, n);
            }

            return end();
        }

        hsh_reference get_hasher()
        {
            const auto &t = *this;
            return const_cast<hsh_reference>(t.get_hasher());
        }

        hsh_const_reference get_hasher() const
        {
            return m_hasher;
        }

        allocator_reference get_allocator()
        {
            const auto &t = *this;
            return const_cast<allocator_reference>(t.get_allocator());
        }

        allocator_const_reference get_allocator() const
        {
            return m_allocator;
        }

        comparator_reference get_comparator()
        {
            const auto &t = *this;
            return const_cast<comparator_reference>(t.get_comparator());
        }

        comparator_const_reference get_comparator() const
        {
            return m_comparator;
        }

        void clear()
        {
            destructor_helper(m_root);
            m_size = 0;
        }

        RedBlackTree &operator=(const RedBlackTree &other)
        {
            if (&other != this)
            {
                this->clear();
                for (auto &i : other)
                {
                    this->insert(i);
                }
                this->m_allocator = other.m_allocator;
            }
            return *this;
        }

        RedBlackTree &operator=(RedBlackTree &&other)
        {
            if (&other != this)
            {
                this->clear();
                this->m_root = other.m_root;
                other.m_root = other.null();
                m_size = other.size();
                other.m_size = 0;
                move_helper(other, m_root);
                this->m_allocator = other.m_allocator;
            }
            return *this;
        }
    };

} // namespace hls

#endif
