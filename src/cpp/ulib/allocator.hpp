#ifndef _ALLOCATOR_HPP_
#define _ALLOCATOR_HPP_

#include <stddef.h>
#include "macros.hpp"

template<typename T>
class Allocator {
    SET_USING_CLASS(T, type);
    public:

    Allocator() {
        m_i = 0;
    }

    size_t m_i;

    template<typename ...Args>
    type_ptr create(Args... args) {
        type_ptr v = allocate();
        if(v != nullptr) {
            new(v)type(std::forward<Args>(args)...);
        }

        return v;
    }

    void destroy(type_const_ptr p) {
        if(p == nullptr)
            return;

        type_ptr p_nc = const_cast<type_ptr>(p);
        (*p_nc).~type();
        deallocate(p_nc);
    }

    type_ptr allocate() {
        return reinterpret_cast<type_ptr>(malloc(sizeof(type)));
    }
    
    void deallocate(type_const_ptr p) {
        if(p == nullptr)
            return;
        type_ptr p_nc = const_cast<type_ptr>(p);
        free(p_nc);
    }
};

#endif