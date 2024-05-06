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

#ifndef _MAP_HPP_
#define _MAP_HPP_

#include "misc/utilities.hpp"
#include "ulib/pair.hpp"
#include "ulib/rb_tree.hpp"

namespace hls {

template <typename P> class MapHash {
    using pair = hls::Pair<typename P::first_type, typename P::second_type>;
    using T = pair::first_type;
    using U = pair::second_type;
    SET_USING_CLASS(pair, type);
    SET_USING_CLASS(size_t, hash_result);

    Hash<typename pair::first_type> m_h;

  public:
    size_t operator()(type_const_reference v) const {
        return m_h(v.first);
    }

    const Hash<typename pair::first_type> &get_T_hasher() const {
        return m_h;
    }
};

template <typename Key, typename Value, template <typename> class Allocator>
class Map : RedBlackTree<hls::Pair<Key, Value>, MapHash, LessComparator, Allocator> {
    SET_USING_CLASS(Key, key);
    SET_USING_CLASS(Value, value);
    using PR = hls::Pair<key, value>;
    SET_USING_CLASS(PR, pair);
    using rb_tree = RedBlackTree<hls::Pair<Key, Value>, MapHash, LessComparator, Allocator>;
    EXTRACT_SUB_USING_T_CLASS(rb_tree, node, node);
    EXTRACT_SUB_USING_T_CLASS(rb_tree, const_iterator, iterator);
    EXTRACT_SUB_USING_T_CLASS(rb_tree, const_reverse_iterator, reverse_iterator);

  public:
    Map() = default;
    ~Map() = default;
    Map(const Map &other) : Map() {
        *this = other;
    }

    Map(Map &&other) : Map() {
        *this = hls::move(other);
    }

    using rb_tree::empty;
    using rb_tree::max_size;
    using rb_tree::size;

    bool contains(key_const_reference k) const {
        auto &h = rb_tree::get_hasher().get_T_hasher();
        return rb_tree::contains(h(k));
    }

    iterator find(key_const_reference k) const {
        const auto &h = rb_tree::get_hasher().get_T_hasher();
        if (rb_tree::contains(h(k))) {
            return rb_tree::template build_iterator<iterator>(rb_tree::get_node(h(k)));
        }

        return end();
    }

    // TODO: make template of it, so we can pass rvalues
    iterator insert(key_const_reference k, value_const_reference v) {
        auto &h = rb_tree::get_hasher().get_T_hasher();
        if (rb_tree::contains(h(k))) {
            return rb_tree::template build_iterator<iterator>(rb_tree::get_node(h(k)));
        }

        pair p = {k, v};
        return rb_tree::template build_iterator<iterator>(rb_tree::insert(p));
    }

    void remove(key_const_reference k) {
        auto &h = rb_tree::get_hasher().get_T_hasher();
        auto hash = h(k);
        rb_tree::remove(k);
    }

    void erase(iterator_const_reference it) {
        if (!rb_tree::is_tree_iterator(it))
            return;
        if (it == end())
            return;

        auto &h = rb_tree::get_hasher().get_T_hasher();
        auto hash = h(it->first);
        rb_tree::remove(hash);
    }

    value_reference operator[](key_const_reference k) {
        auto &h = rb_tree::get_hasher().get_T_hasher();
        if (!rb_tree::contains(h(k))) {
            pair p = {k, {}};
            rb_tree::insert(p);
        }
        node_ptr n = rb_tree::get_node(h(k));
        return n->get_data().second;
    }

    iterator lower_bound(key_const_reference k) const {
        auto &h = rb_tree::get_hasher().get_T_hasher();
        return rb_tree::template build_iterator<iterator>(rb_tree::equal_or_greater(h(k)));
    }

    iterator upper_bound(key_const_reference k) const {
        auto &h_pair = rb_tree::get_hasher();
        auto &h = rb_tree::get_hasher().get_T_hasher();
        auto &c = rb_tree::comparator();

        node_const_ptr n = rb_tree::equal_or_greater(h(k));

        if (n == rb_tree::t_null() || c(h_pair(n->get_data()), h(k)))
            return rb_tree::template build_iterator<iterator>(n);

        return rb_tree::template build_iterator<iterator>(rb_tree::get_in_order_successor(n));
    }

    value_const_reference at(key_const_reference k) const {
        auto &h = rb_tree::get_hasher().get_T_hasher();
        node_const_ptr n = rb_tree::get_node(h(k));

        if (n == rb_tree::t_null()) {
            // FAIL MISERABLY
        }
        return n->get_data().second;
    }

    iterator begin() const {
        return rb_tree::begin();
    }

    iterator end() const {
        return rb_tree::end();
    }

    reverse_iterator rbegin() const {
        return rb_tree::rbegin();
    }

    reverse_iterator rend() const {
        return rb_tree::rend();
    }

    Map &operator=(Map &&other) {
        if (&other != this) {
            rb_tree *a = this;
            rb_tree *b = &other;
            *a = hls::move(*b);
        }
        return *this;
    }

    Map &operator=(const Map &other) {
        if (&other != this) {
            rb_tree::clear();
            for (auto &p : other) {
                insert(p.first, p.second);
            }
        }
        return *this;
    }
};

} // namespace hls

#endif