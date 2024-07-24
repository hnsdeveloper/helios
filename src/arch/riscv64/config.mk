# Configuration for riscv64
MACROS := $(MACROS) -DARCH=riscv64
CFLAGS := $(CFLAGS) -march=rv64gc -mabi=lp64d -mno-relax
CXXFLAGS := $(CXXFLAGS) -march=rv64gc -mabi=lp64d -mno-relax
LDFLAGS := $(LDFLAGS) -m elf64lriscv --no-relax
SV39_MASK := 0x8000000000000000
SV48_MASK := 0x9000000000000000
SV39_KBASE_ADDRESS := 0xFFFFFFFFC0000000
SV48_KBASE_ADDRESS := 0xFFFFFFFFC0000000

ifndef SYSTEM
    $(error SYSTEM is not set. Required for arch $(ARCH).)
endif

ifeq ($(SYSTEM), visionfive_2)
    MACROS += -DRISCV_MEM_MODE_MASK=$(SV39_MASK) -DRISCV_MEM_ORIGIN=0x40200000 -DSV39
else ifeq ($(SYSTEM), qemu_sv39)
    MACROS += -DRISCV_MEM_MODE_MASK=$(SV39_MASK) -DRISCV_MEM_ORIGIN=0x84000000 -DSV39
else ifeq ($(SYSTEM), qemu_sv48)
    MACROS += -DRISCV_MEM_MODE_MASK=$(SV48_MASK) -DRISCV_MEM_ORIGIN=0x84000000 -DSV48
else
    $(error SYSTEM $(SYSTEM) not supported.)
endif
