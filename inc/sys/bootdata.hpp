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
#ifndef _BOOTDATA__HPP_
#define _BOOTDATA__HPP_

#include "misc/types.hpp"

namespace hls
{

    struct PageTable;

    struct bootinfo
    {
        size_t argc;
        const char **argv;
        size_t used_bootpages;
        PageTable *p_kernel_table;
        PageTable *v_scratch;
        byte *p_lowkernel_start;
        byte *p_lowkernel_end;
        byte *v_highkernel_start;
        byte *v_highkernel_end;
        byte *p_kernel_physical_end;
        byte *v_device_drivers_begin;
        byte *v_device_drivers_end;
    };
} // namespace hls

#endif