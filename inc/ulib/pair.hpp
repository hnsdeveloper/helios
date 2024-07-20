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

#ifndef _PAIR_HPP_
#define _PAIR_HPP_

#include "misc/utilities.hpp"
#include <type_traits>

namespace hls
{

    template <typename T, typename U>
    class Pair
    {

        SET_USING_CLASS(T, first_type);
        SET_USING_CLASS(U, second_type);

      public:
        Pair() = default;
        Pair(T &&a, U &&b) : first(hls::move(a)), second(hls::move(b)) {};
        Pair(const T &a, const U &b) : first(a), second(b) {};
        Pair(Pair &&p) : first(hls::move(p.first)), second(hls::move(p.second)) {};
        Pair(const Pair &p) : first(p.first), second(p.second) {};
        ~Pair() = default;

        first_type first;
        second_type second;

        Pair &operator=(Pair &other)
        {
            first = other.first;
            second = other.second;
        }

        Pair &operator=(Pair &&other)
        {
            first = hls::move(other.first);
            second = hls::move(other.second);
        }
    };

    template <typename T, typename U>
    auto make_pair(T &&first, U &&second)
    {
        using first_type = std::remove_cvref_t<decltype(first)>;
        using second_type = std::remove_cvref_t<decltype(second)>;

        return Pair<first_type, second_type>(hls::move(first), hls::move(second));
    };

} // namespace hls

#endif
