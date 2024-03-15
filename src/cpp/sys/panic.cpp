#include "sys/panic.hpp"
#include "sys/print.hpp"

using namespace hls;

struct __attribute__((packed)) _reg_as_data {
  uint64_t x1;
  uint64_t x2;
  uint64_t x3;
  uint64_t x4;
  uint64_t x5;
  uint64_t x6;
  uint64_t x7;
  uint64_t x8;
  uint64_t x9;
  uint64_t x10;
  uint64_t x11;
  uint64_t x12;
  uint64_t x13;
  uint64_t x14;
  uint64_t x15;
  uint64_t x16;
  uint64_t x17;
  uint64_t x18;
  uint64_t x19;
  uint64_t x20;
  uint64_t x21;
  uint64_t x22;
  uint64_t x23;
  uint64_t x24;
  uint64_t x25;
  uint64_t x26;
  uint64_t x27;
  uint64_t x28;
  uint64_t x29;
  uint64_t x30;
  uint64_t x31;
  uint64_t pc;
};

struct hart {
  union {
    _reg_as_data data;
    uint64_t array[32];
  } reg;
};

extern "C" void print_registers(hart *h) {

  for (size_t i = 0; i < 32; ++i) {
    kprint("x{}: {} ", i,
           i == 0 ? (void *)(0)
                  : reinterpret_cast<void *>(h->reg.array[i - 1]));
    if ((i + 1) % 4 == 0) {
      strprintln("");
    }
  }
  kprintln("pc: {}", reinterpret_cast<void *>(h->reg.data.pc));
}
