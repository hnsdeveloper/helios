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
#include "ulib/pair.hpp"

namespace hls
{
    template <typename ClassType>
    class Singleton
    {
        SET_USING_CLASS(ClassType, class_t);

        static Pair<class_t_ptr, bool *> mem_get()
        {
            static bool initialized = false;
            alignas(class_t) static byte mem[sizeof(class_t)];

            return {reinterpret_cast<class_t_ptr>(mem), &initialized};
        }

      public:
        static class_t_reference get_global_instance()
        {
            auto p = mem_get();
            if (*(p.second) == false)
                PANIC("Attempting to use unitialized singleton.");
            return *(p.first);
        }

        template <typename... Args>
        static void initialize_global_instance(Args... args)
        {
            auto p = mem_get();
            if (*(p.second) == false)
            {
                new (p.first) class_t(hls::forward<Args>(args)...);
                *(p.second) = true;
            }
        }
    };
} // namespace hls

#endif
