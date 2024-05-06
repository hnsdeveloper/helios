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

#include "sys/opensbi.hpp"

namespace hls {

void sbi_call(uint64_t extension, uint64_t function_id, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4,
              uint64_t arg5, uint64_t arg6, uint64_t &error, uint64_t &value) {
    register uint64_t a0 asm("a0") = arg1;
    register uint64_t a1 asm("a1") = arg2;
    register uint64_t a2 asm("a2") = arg3;
    register uint64_t a3 asm("a3") = arg4;
    register uint64_t a4 asm("a4") = arg5;
    register uint64_t a5 asm("a5") = arg6;
    register uint64_t a6 asm("a6") = function_id;
    register uint64_t a7 asm("a7") = extension;

    asm volatile("ecall" : "+r"(a0), "+r"(a1) : "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a6), "r"(a7) : "memory");
    error = a0;
    value = a1;
}

void opensbi_putchar(char c) {
    uint64_t a = 0;
    sbi_call(0x1u, 0x0, c, 0, 0, 0, 0, 0, a, a);
};

void opensbi_set_timer(uint64_t time) {
    uint64_t a = 0;
    sbi_call(0x0, 0, time, 0, 0, 0, 0, 0, a, a);
}

}; // namespace hls
