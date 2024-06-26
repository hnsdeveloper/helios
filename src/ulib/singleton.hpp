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

#ifndef _SINGLETON_HPP_
#define _SINGLETON_HPP_

#include "misc/macros.hpp"
#include "misc/new.hpp"
#include "misc/types.hpp"
#include "misc/utilities.hpp"
#include "sys/mem.hpp"
#include "sys/panic.hpp"
#include "sys/print.hpp"
#include <type_traits>

namespace hls
{
    template <typename ClassType, size_t type_size>
        requires(is_power_of_two(type_size))
    class Singleton
    {
        SET_USING_CLASS(ClassType, class_t);
        alignas(type_size) static inline byte s_mem[type_size] = {0};

        static bool set_and_get_initialized_internal(bool v = false)
        {
            static bool initialized = false;
            if (v)
                initialized = true;
            return initialized;
        }

      public:
        static class_t_reference instance()
        {
            // static_assert(type_size >= sizeof(class_t));
            if (!is_initialized())
            {
                PANIC("Attempting to get instance of unitialized class.");
            }
            return *reinterpret_cast<class_t_ptr>(align_forward(s_mem, alignof(class_t)));
        }

        template <typename... Args>
        static void initialize_instance(Args &&...args)
        {
            // static_assert(type_size >= sizeof(class_t));

            if (!is_initialized())
            {
                new (align_forward(s_mem, alignof(class_t))) class_t(hls::forward<Args>(args)...);
                set_and_get_initialized_internal(true);
            }
            else
            {
                PANIC("Attempting to initialize already initialized class");
            }
        }

        static bool is_initialized()
        {
            return set_and_get_initialized_internal();
        }
    };

} // namespace hls

#endif
