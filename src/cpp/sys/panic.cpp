#include "sys/panic.hpp"
#include "sys/print.hpp"

using namespace hls;

struct __attribute__((packed)) _reg_as_ptr {
  void *x1;
  void *x2;
  void *x3;
  void *x4;
  void *x5;
  void *x6;
  void *x7;
  void *x8;
  void *x9;
  void *x10;
  void *x11;
  void *x12;
  void *x13;
  void *x14;
  void *x15;
  void *x16;
  void *x17;
  void *x18;
  void *x19;
  void *x20;
  void *x21;
  void *x22;
  void *x23;
  void *x24;
  void *x25;
  void *x26;
  void *x27;
  void *x28;
  void *x29;
  void *x30;
  void *x31;
  void *pc;
};

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
};

struct hart {
  union {
    _reg_as_data _data;
    _reg_as_ptr _ptr;
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
  kprintln("pc: {}", h->reg._ptr.pc);
}
