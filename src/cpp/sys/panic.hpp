/*---------------------------------------------------------------------------------
MIT License

Copyright (c) 2022 Helio Nunes Santos

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

#ifndef _PANIC_HPP_
#define _PANIC_HPP_

#include "print.hpp"

struct hart;

extern "C" void print_registers(hart *);
extern "C" void _die();

#define REGPRINT()                                                             \
  asm("1:                                                                     \
       addi sp, sp, -256;           \
       sd x1, 0(x2);                \
       sd x3, 16(x2);               \
       sd x4, 24(x2);               \
       sd x5, 32(x2);               \
       sd x6, 40(x2);               \
       sd x7, 48(x2);               \
       sd x8, 56(x2);               \
       sd x9, 64(x2);               \
       sd x10, 72(x2);              \
       sd x11, 80(x2);              \
       sd x12, 88(x2);              \
       sd x13, 96(x2);              \
       sd x14, 104(x2);             \
       sd x15, 112(x2);             \
       sd x16, 120(x2);             \
       sd x17, 128(x2);             \
       sd x18, 136(x2);             \
       sd x19, 144(x2);             \
       sd x20, 152(x2);             \
       sd x21, 160(x2);             \
       sd x22, 168(x2);             \
       sd x23, 176(x2);             \
       sd x24, 184(x2);             \
       sd x25, 192(x2);             \
       sd x26, 200(x2);             \
       sd x27, 208(x2);             \
       sd x28, 216(x2);             \
       sd x29, 224(x2);             \
       sd x30, 232(x2);             \
       sd x31, 240(x2);             \
       add a0, x0, sp;              \
       addi a0, a0, 256;            \
       sd a0, 8(sp);                \
       la a0, 1b;                   \
       sd a0, 132(x2);              \
       add a0, x0, sp;              \
       call print_registers;        \
");

#define PANIC(msg)                                                             \
  REGPRINT();                                                                  \
  kprintln("At {}.\r\n{}\r\nLine: {}", __FILE__, __PRETTY_FUNCTION__,          \
           __LINE__);                                                          \
  kprintln("Panic message: {}", #msg);                                         \
  _die();

#endif