#ifndef _STACK_HPP_
#define _STACK_HPP_

#include "double_list.hpp"

template<typename T, template<class, class> class ds = DoubleList, template<class> class Allocator>
class Stack {
    SET_USING_CLASS(T, type);    
    using _underlying = ds<type, Allocator>;
    SET_USING_CLASS(_underlying, Und);
    EXTRACT_SUB_USING_T_CLASS(Und, allocator, allocator);
public:

    using ds::size;
    using ds::empty;
    using ds::pop_back;

    void push_back(type_const_reference v) {
        ds::push_back(v);
    }

    type_reference top() {
        const auto& as_const= *this;
        return const_cast<type_reference>(as_const.top());
    }

    type_const_reference top() {
        if(empty())
            PANIC("Requesting element from empty stack.");
        return *(--ds::end());
    }

};

#endif