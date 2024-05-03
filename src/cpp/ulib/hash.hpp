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

#ifndef _HASH_HPP_
#define _HASH_HPP_

#include "include/macros.hpp"
#include "include/types.hpp"

namespace hls {

template<typename T>
class Hash {
    SET_USING_CLASS(T, type);
    SET_USING_CLASS(size_t, hash_result);
    
    public:
    
    hash_result operator()(type_const_reference v) const {
        return v;
    }
};

template<typename T>
class Hash<T*> {
    SET_USING_CLASS(T*, type);
    SET_USING_CLASS(uintptr_t, hash_result);

    public:

    hash_result operator()(type_const_reference v) const {
        return reinterpret_cast<hash_result>(v);
    }
};

}

#endif