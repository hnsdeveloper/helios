#ifndef __DOUBLE_LIST_HPP_
#define __DOUBLE_LIST_HPP_

#include "node.hpp"

template<typename T>
class DoubleListNode : protected Node<T, 2> {
    using nd = Node<T, 2>;
    SET_USING_CLASS(nd, node_base);
    SET_USING_CLASS(DoubleListNode, node);
    EXTRACT_SUB_USING_T_CLASS(node_base, type, type);

    public:

    DoubleListNode() = default;

    DoubleListNode(type_const_reference v, node_ptr p, node_ptr n) {
        set_data(v);
        set_previous(p);
        set_next(n);

        p->set_next(this);
        n->set_previous(this);
    }

    ~DoubleListNode() = default;

    using node_base::set_data;
    using node_base::get_data;

    void set_previous(node_ptr n) {
        node:: template set_node<0>(n);
    }

    void set_next(node_ptr n) {
        node:: template set_node<1>(n);
    }

    node_ptr get_previous() {
        const auto& t = *this;
        return const_cast<node_ptr>(t.get_previous());
    }

    node_const_ptr get_previous() const {
        return reinterpret_cast<node_const_ptr>(node:: template get_node<0>());
    }

    node_ptr get_next() {
        const auto& t = *this;
        return const_cast<node_ptr>(t.get_next());
    }

    node_const_ptr get_next() const {
        return reinterpret_cast<node_const_ptr>(node:: template get_node<1>());
    }
    
};

template<typename T, template <typename> class Allocator> 
class DoubleList {
    using nd = DoubleListNode<T>;
    SET_USING_CLASS(nd, node);
    using _alloc = Allocator<node>;
    SET_USING_CLASS(_alloc, allocator);
    EXTRACT_SUB_USING_T_CLASS(node, type, type);

    template<bool reverse, bool cnst = false>
    class DoubleListIterator {
        const DoubleList* m_list;
        node_const_ptr m_n;
        
        DoubleListIterator(const DoubleList* list, node_const_ptr n) : m_list(list), m_n(n) {};

        template<bool c>
        struct wrapper {
            node_const_ptr n;
            wrapper(node_const_ptr node) : n(node) {};
            node_const_ptr get_n() { return n; }
        };

        template<>
        struct wrapper<false> {
            node_const_ptr n;
            wrapper(node_const_ptr node) : n(node) {};
            node_ptr get_n() { return const_cast<node_ptr>(n); }
        };

        node_ptr get_node() {
            const auto& t = *this;
            return const_cast<node_ptr>(t.get_node());
        }
        node_const_ptr get_node() const {
            return m_n;
        }

        const DoubleList* get_list() {
            return m_list;
        }

        public:

        DoubleListIterator(const DoubleListIterator& other) : m_list(other.m_list), m_n(other.m_n) {};

        auto& operator*() const {
            auto n = wrapper<cnst>(m_n).get_n();
            return n->get_data();
        }

        auto operator->() const {
            auto n = wrapper<cnst>(m_n).get_n();
            return &(n->get_data());
        }

        DoubleListIterator& operator++() {
            if constexpr (reverse) {
                m_n = m_n->get_previous();
            } else {
                m_n = m_n->get_next();
            }
            return *this;
        }

        DoubleListIterator operator++(int) {
            auto other = *this;
            ++(*this);
            return other;
        }

        DoubleListIterator& operator--() {
            if constexpr(reverse) {
                m_n = m_n->get_next();
            }
            else {
                m_n = m_n->get_previous();
            }
            return *this;
        }

        DoubleListIterator operator--(int) {
            auto other = *this;
            --(*this);
            return other;
        }

        DoubleListIterator& operator=(const DoubleListIterator& other) {
            m_list = other.m_list;
            m_n = other.m_n;
            return *this;
        }

        bool operator==(const DoubleListIterator& other) const {
            return m_n == other.m_n && m_list == other.m_list;
        }

        bool operator!=(const DoubleListIterator& other) const {
            return !(*(this) == other);
        }


        friend class DoubleList;
    };

    node m_b;
    node m_e;
    node_ptr m_head = nullptr;
    allocator m_allocator;
    size_t m_size = 0;

    using it = DoubleListIterator<false,false>;
    using rit = DoubleListIterator<true, false>;
    using cit = DoubleListIterator<false, true>;
    using crit = DoubleListIterator<true, true>;

    SET_USING_CLASS(it, iterator);
    SET_USING_CLASS(rit, reverse_iterator);
    SET_USING_CLASS(cit, const_iterator);
    SET_USING_CLASS(crit, const_reverse_iterator);

    public:

    DoubleList() {
        m_b.set_next(&m_e);
        m_e.set_previous(&m_b);
        m_head = &m_e;
    }

    ~DoubleList() {
        clear();
    }

    iterator insert(iterator at, type_const_reference v) {
        node_ptr n = nullptr;

        if(at->get_list() == this) {
            bool head_upd = at == begin();
            node_ptr a = at.get_node();
            node_ptr b = a->get_previous();
            n = m_allocator.create(v, b, a);
            if(head_upd)
                m_head = n;

            ++m_size;
        }
        return iterator(this, n);
    }

    iterator remove(iterator at) {
    
        if(at->get_node() == &m_b || at->get_node() == &m_e || at->get_list() != this)
            return end();

        node_ptr b = nullptr;
        node_ptr o = at.get_node();
        node_ptr a = nullptr;
        b = o->get_previous();
        a = o->get_next();

        b->set_next(a);
        a->set_previous(b);

        m_allocator.destroy(o);
        --m_size;
    }

    iterator push_back(type_const_reference v) {
        return insert(end(), v);        
    }

    void pop_back() {
        if(size())
            remove(--end());
    }

    iterator push_front(type_const_reference v) {
        return insert(begin(), v);
    }

    void pop_front() {
        if(size())
            remove(begin());
    }

    iterator begin() {
        if(m_head)
            return iterator(this, m_head);
        
        return end();
    }

    iterator end() {
        return iterator(this, &m_e);
    }

    reverse_iterator rbegin() {
        if(m_head) 
            return reverse_iterator(this, m_e->get_previous());
        return rend();
    }

    reverse_iterator rend() {
        return reverse_iterator(this, &m_b);
    }

    const_iterator begin() const {
        if(m_head)
            return const_iterator(this, m_head);
        
        return end();
    }

    const_iterator end() const {
        return const_iterator(this, &m_e);
    }

    const_reverse_iterator rbegin() const {
        if(m_head) 
            return const_reverse_iterator(this, m_e->get_previous());
        return rend();
    }

    const_reverse_iterator rend() const {
        return const_reverse_iterator(this, &m_b);
    }
    
    size_t size() const {
        return m_size;
    }

    void clear() {
        while(size()) {
            remove(begin());
        }
    }

};

#endif