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
#include "arch/riscv64gc/plat_def.hpp"
#include "sys/print.hpp"

using namespace hls;

extern "C" void _setup_trap_handling();

namespace hls
{

    void setup_trap_handling()
    {
        _setup_trap_handling();
    }

    extern "C" void traphandler(registers *)
    {

        uintreg_t scause = 0;
        asm volatile("csrrw %1, scause, %0" : "=r"(scause) : "r"(scause));

        const bool is_sync = (scause >> 63);
        const uintreg_t cause = (scause << 1) >> 1; // Discards the MSB

        if (is_sync)
        {
            switch (cause)
            {

            default:
                PANIC("Unhandled synchronous trap cause.");
            }
        }
        else
        {
            switch (cause)
            {

            default:
                PANIC("Unhandled asynchronous trap cause.");
            }
        }
    }

} // namespace hls
