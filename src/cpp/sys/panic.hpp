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

/**
 * @brief Prints a stack trace, where caller addresses are printed.
 *
 */
extern "C" void stack_trace();

/**
 * @brief Kills the system.
 * @todo Implement so that when the system is fully loaded, it saves a dump of
 * everything and reboots. If the system is not fully loaded, then it should
 * print to console and freeze.
 */
extern "C" void die();

/**
 * @brief Macro used to print current registers values.
 *
 */
#define PRINT_REGISTERS()                                                      \
  {                                                                            \
    asm volatile("1:"                                                          \
                 "addi sp, sp, -288;"                                          \
                 "sd   x10, 0(sp);"                                            \
                 "la   x10, 1b;"                                               \
                 "add  x10, x10, -4;"                                          \
                 "sd   x10, 8(sp);"                                            \
                 "jal  x10, _print_registers;");                               \
  }

/**
 * @brief Macro used for kernel panics. Prints registers values, shows a panic
 * message, calling location and stack trace.
 *
 */
#define PANIC(msg)                                                             \
  PRINT_REGISTERS();                                                           \
  kprintln("Panic message: {}", #msg);                                         \
  kprintln("At {} {} line: {}", __FILE__, __PRETTY_FUNCTION__, __LINE__);      \
  stack_trace();                                                               \
  die();

#endif