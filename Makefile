PROJECT_ROOT := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
SRC_DIR := $(PROJECT_ROOT)src/
INCLUDE_DIR := $(SRC_DIR)
SUBDIRS := $(wildcard */.)

ifndef BUILD_DIR
    BUILD_DIR := $(PROJECT_ROOT)build/
endif

ifdef HELIOS_DEBUG
    ifeq ($(HELIOS_DEBUG), 1)
        EXTRAFLAGS := $(EXTRAFLAGS) -g
        MACROS := -DDEBUG $(MACROS)
    endif
endif

ifndef ARCH
    $(error ARCH is not set)
else
    ifeq ($(ARCH), riscv64)
        CPPFLAGS := $(CPPFLAGS) -march=rv64gc -mabi=lp64d -mcmodel=medany
        SV39_MASK := 0x8000000000000000
        SV48_MASK := 0x9000000000000000
        SV39_KBASE_ADDRESS := 0xFFFFFFFFC0000000
        SV48_KBASE_ADDRESS := 0xFFFFFFFFC0000000
        ARCH_FOLDER := ./src/arch/riscv64gc
        MAKES := $(ARCH_FOLDER)
        ifndef SYSTEM
            $(error SYSTEM is not set. Required for arch $(ARCH).)
        else ifeq ($(SYSTEM), visionfive_2)
            MACROS := $(MACROS) -DRISCV_MEM_MODE_MASK=$(SV39_MASK) -DRISCV_MEM_ORIGIN=0x40200000 -DSV39
        else ifeq ($(SYSTEM), qemu_sv39)
            MACROS := $(MACROS) -DRISCV_MEM_MODE_MASK=$(SV39_MASK) -DRISCV_MEM_ORIGIN=0x84000000 -DSV39
        else ifeq ($(SYSTEM), qemu_sv48)
            MACROS := $(MACROS) -DRISCV_MEM_MODE_MASK=$(SV48_MASK) -DRISCV_MEM_ORIGIN=0x84000000 -DSV48
		else
            $(error SYSTEM $(SYSTEM) not supported.)
		endif
	endif
endif


MAKES := $(MAKES) $(shell find ./src -not \( -path ./src/arch -prune \) -name "Makefile" | xargs dirname )

CPPFLAGS := $(CPPFLAGS) -c -fno-omit-frame-pointer -march=rv64gc -std=c++20  -ffreestanding -nostdlib -fno-exceptions \
-fno-rtti -fno-asynchronous-unwind-tables -fno-use-cxa-atexit -Wall -Wextra -Werror

EXTRAFLAGS := $(EXTRAFLAGS)

MACROS := $(MACROS) -DBOOTPAGES=32

ifdef CXXPREFIX
    CC  := $(CXXPREFIX)gcc
    CXX := $(CXXPREFIX)g++
    AR  := $(CXXPREFIX)ar
    LD	:= $(CXXPREFIX)ld
else
    CC  := clang
    CXX := clang++
    AR  := llvm-ar
    LD	:= lld
endif

export BUILD_DIR
export INCLUDE_DIR
export SRC_DIR
export PROJECT_ROOT
export CPPFLAGS
export EXTRAFLAGS
export MACROS
export CC
export CXX
export AR
export LD

.PHONY: dep
.PHONY: helios
.PHONY: helios.bin

helios : $(BUILD_DIR)helios
helios.bin : $(BUILD_DIR)helios.bin


$(BUILD_DIR)helios : dep $(BUILD_DIR)link.lds
	@echo "Linking object files."
	@$(LD) $(BUILD_DIR)*.o -nostdlib --gc-sections -T$(BUILD_DIR)link.lds -o $(BUILD_DIR)helios
	
$(BUILD_DIR)helios.bin : $(BUILD_DIR)helios
	@echo "Generating kernel binary"
	@$(CXXPREFIX)objcopy -O binary $(BUILD_DIR)helios $(BUILD_DIR)helios.bin

dep :
	@mkdir -p build
	@for folder in $(MAKES); do $(MAKE) -C $$folder || exit; done

$(BUILD_DIR)link.lds : $(ARCH_FOLDER)/link.lds
	@$(CXXPREFIX)cpp $(MACROS) -xc -P -C $<  -o $@
	
clean :
	@$(RM) build/*.o

fclean :
	@$(RM) -rf build/



  
#driverframework.o: src/dev/driverframework.cpp \
  src/dev/driverframework.hpp src/misc/macros.hpp src/misc/types.hpp \
  src/sys/kmalloc.hpp src/misc/new.hpp src/misc/utilities.hpp \
  src/misc/typetraits.hpp src/ulib/double_list.hpp src/ulib/node.hpp \
  src/misc/symbols.hpp
#traphandler.o: src/traphandler/traphandler.cpp \
  src/arch/riscv64gc/plat_def.hpp src/misc/macros.hpp src/misc/types.hpp \
  src/sys/print.hpp src/misc/limits.hpp src/misc/concepts.hpp \
  src/misc/typetraits.hpp src/sys/mem.hpp src/sys/opensbi.hpp
