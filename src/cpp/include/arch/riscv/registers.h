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

#ifndef _REGISTERS_H_
#define _REGISTERS_H_

#include "include/types.h"

namespace hls {
enum class MCSR : uint16_t {
  mvendorid = 0xF11,
  marchid = 0xF12,
  mimpid = 0xF13,
  mhartid = 0xF14,
  mstatus = 0x300,
  misa = 0x301,
  medeleg = 0x302,
  mideleg = 0x303,
  mie = 0x304,
  mtvec = 0x305,
  mcounteren = 0x306,
  mscratch = 0x340,
  mepc = 0x341,
  mcause = 0x342,
  mtval = 0x343,
  mip = 0x344,
  pmpcfg0 = 0x3A0,
  pmpcfg1 = 0x3A1,
  pmpcfg2 = 0x3A2,
  pmpcfg3 = 0x3A3,
  pmpaddr0 = 0x3B0,
  pmpaddr1 = 0x3B1,
  pmpaddr2 = 0x3B2,
  pmpaddr3 = 0x3B3,
  pmpaddr4 = 0x3B4,
  pmpaddr5 = 0x3B5,
  pmpaddr6 = 0x3B6,
  pmpaddr7 = 0x3B7,
  pmpaddr8 = 0x3B8,
  pmpaddr9 = 0x3B9,
  pmpaddr10 = 0x3BA,
  pmpaddr11 = 0x3BB,
  pmpaddr12 = 0x3BC,
  pmpaddr13 = 0x3BD,
  pmpaddr14 = 0x3BE,
  pmpaddr15 = 0x3BF,
  mcycle = 0xB00,
  minstret = 0XB02,
  mhpmcounter3 = 0XB03,
  mhpmcounter4 = 0XB04,
  mhpmcounter5 = 0XB05,
  mhpmcounter6 = 0XB06,
  mhpmcounter7 = 0XB07,
  mhpmcounter8 = 0XB08,
  mhpmcounter9 = 0XB09,
  mhpmcounter10 = 0XB0A,
  mhpmcounter11 = 0XB0B,
  mhpmcounter12 = 0XB0C,
  mhpmcounter13 = 0XB0D,
  mhpmcounter14 = 0XB0E,
  mhpmcounter15 = 0XB0F,
  mhpmcounter16 = 0XB10,
  mhpmcounter17 = 0XB11,
  mhpmcounter18 = 0XB12,
  mhpmcounter19 = 0XB13,
  mhpmcounter20 = 0XB14,
  mhpmcounter21 = 0XB15,
  mhpmcounter22 = 0XB16,
  mhpmcounter23 = 0XB17,
  mhpmcounter24 = 0XB18,
  mhpmcounter25 = 0XB19,
  mhpmcounter26 = 0XB1A,
  mhpmcounter27 = 0XB1B,
  mhpmcounter28 = 0XB1C,
  mhpmcounter29 = 0XB1D,
  mhpmcounter30 = 0XB1E,
  mhpmcounter31 = 0XB1F,
  mhpmevent3 = 0X323,
  mhpmevent4 = 0X324,
  mhpmevent5 = 0X325,
  mhpmevent6 = 0X326,
  mhpmevent7 = 0X327,
  mhpmevent8 = 0X328,
  mhpmevent9 = 0X329,
  mhpmevent10 = 0X32A,
  mhpmevent11 = 0X32B,
  mhpmevent12 = 0X32C,
  mhpmevent13 = 0X32D,
  mhpmevent14 = 0X32E,
  mhpmevent15 = 0X32F,
  mhpmevent16 = 0X330,
  mhpmevent17 = 0X331,
  mhpmevent18 = 0X332,
  mhpmevent19 = 0X333,
  mhpmevent20 = 0X334,
  mhpmevent21 = 0X335,
  mhpmevent22 = 0X336,
  mhpmevent23 = 0X337,
  mhpmevent24 = 0X338,
  mhpmevent25 = 0X339,
  mhpmevent26 = 0X33A,
  mhpmevent27 = 0X33B,
  mhpmevent28 = 0X33C,
  mhpmevent29 = 0X33D,
  mhpmevent30 = 0X33E,
  mhpmevent31 = 0X33F,
  mcontext = 0x7A8
};

}

#endif