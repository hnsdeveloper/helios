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

#ifndef _NEW_HPP_
#define _NEW_HPP_

#include "misc/macros.hpp"
#include "misc/types.hpp"

// Taken from stack overflow. GCC requires these functions to initialize local
// static variables. Note that this offers no race protection, thus it is only
// safe for single threaded code!
__extension__ typedef int __guard __attribute__((mode(__DI__)));

extern "C" LKERNELFUN int __cxa_guard_acquire(__guard *g);

extern "C" LKERNELFUN void __cxa_guard_release(__guard *g);

extern "C" LKERNELFUN void __cxa_guard_abort(__guard *);

extern "C" LKERNELFUN int atexit(void (*)());

// Placement new
extern "C" void *operator new(size_t, void *w);

#endif