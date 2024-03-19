#include "sys/panic.hpp"
#include "sys/print.hpp"

using namespace hls;

struct __attribute__((packed)) _reg_as_data {
  uint64_t x0;
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

void print_stack_trace(void *address, size_t level) {
  auto print_space = [](size_t level) {
    for (size_t i = 0; i < level; ++i) {
      kprint(" ");
    }
  };

  if (level) {
    print_space(level);
    kprintln("|");
    print_space(level);
    kprint(">");
  }

  kprintln(" Called from {}.", address);
}

extern "C" void print_registers(hart *h) {
  for (size_t i = 0; i < 32; ++i) {
    kprint("x{}: {} ", i, reinterpret_cast<void *>(h->reg.array[i]));
    if ((i + 1) % 4 == 0) {
      strprintln("");
    }
  }
  kprintln("pc: {}", reinterpret_cast<void *>(h->reg.data.pc));
}

extern "C" void stack_trace() {
  void *fp = nullptr;
  size_t level = 0;

  // Lambda to read the return address from the frame pointer
  auto read_ra = [](void *fp) {
    uint64_t *v = reinterpret_cast<uint64_t *>(fp);
    return reinterpret_cast<void *>(*(v - 1) - 4);
  };

  // Lambda to read the next frame pointer in the chain
  auto read_fp = [](void *fp) {
    uint64_t *v = reinterpret_cast<uint64_t *>(fp);
    return reinterpret_cast<void *>(*(v - 2));
  };

  asm("add %0, x0, s0;" : "=r"(fp) : "r"(fp));

  // The first one we skip, given that it will point to the call to
  // stack_trace
  fp = read_fp(fp);

  while (fp != nullptr) {
    print_stack_trace(read_ra(fp), level);
    ++level;
    fp = read_fp(fp);
  }
}

extern "C" void panic_message_print(const char *msg) { kprintln("{}", msg); }
