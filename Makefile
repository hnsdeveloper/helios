PROJECT_ROOT := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
SRC_DIR := $(PROJECT_ROOT)src/
INCLUDE_DIR := $(PROJECT_ROOT)inc/
SUBDIRS := $(wildcard */.)

ifndef OBJ_DIR
    OBJ_DIR := $(PROJECT_ROOT)obj
endif

ifndef LIB_DIR
    LIB_DIR := $(PROJECT_ROOT)lib/
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
        MACROS := $(MACROS) -DARCH=riscv64
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


MAKES := $(MAKES) $(shell find ./src -not \( -path ./src/arch -prune \) -not \( -path ./dep -prune \) -name "Makefile" | xargs dirname )

CPPFLAGS := $(CPPFLAGS) -c -fno-omit-frame-pointer -std=c++20  -ffreestanding -fno-exceptions \
-fno-rtti -fno-asynchronous-unwind-tables -fno-use-cxa-atexit -Wall -Wextra -Werror

EXTRAFLAGS := $(EXTRAFLAGS)

MACROS := $(MACROS) -DBOOTPAGES=32

ifdef CXXPREFIX
    CC  := $(CXXPREFIX)gcc
    CXX := $(CXXPREFIX)g++
    AR  := $(CXXPREFIX)ar
    LD	:= $(CXXPREFIX)ld
else
    $(error CXXPREFIX is not set)
endif

export OBJ_DIR
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
.PHONY: helios.efi


%.o : %.c
	@mkdir -p $(OBJ_DIR)
	@$(CXX) -MM $(CPPFLAGS) $(EXTRAFLAGS) $(MACROS) -I$(INCLUDE_DIR) $*.c > $*.d
	@$(CXX) $(CPPFLAGS) $(EXTRAFLAGS) $(MACROS) -I$(INCLUDE_DIR) -o $@ $^

%.o : %.cpp
	@$(CXX) -MM $(CPPFLAGS) $(EXTRAFLAGS) $(MACROS) -I$(INCLUDE_DIR) $*.cpp > $*.d
	@$(CXX) $(CPPFLAGS) $(EXTRAFLAGS) $(MACROS) -I$(INCLUDE_DIR) -o $@ $^

.PHONY: libs

libs : libfdt

.PHONY: libfdt

LIBFDT_SOURCES := $(wildcard $(PROJECT_ROOT)dep/libfdt/*.c)
LIBFDT_OBJECTS := $(patsubst $(PROJECT_ROOT)dep/libfdt/%.c,$(OBJ_DIR)/%.o,$(LIBFDT_SOURCES))

libfdt : $(LIB_DIR)libfdt.a

$(LIB_DIR)libfdt.a : $(LIBFDT_OBJECTS)
	@mkdir -p $(LIB_DIR)
	@$(AR) rcs -o $@ $<
	
$(LIBFDT_OBJECTS) : $(OBJ_DIR)/%.o : $(LIBFDT_SOURCES)
	@mkdir -p $(OBJ_DIR)
	@$(CXX) $(CPPFLAGS) $(EXTRAFLAGS) $(MACROS) -I$(INCLUDE_DIR) $^

#@$(CXX) -MM $(CPPFLAGS) $(EXTRAFLAGS) $(MACROS) -I$(INCLUDE_DIR) $@.cpp > $^.d


#helios : $(OBJ_DIR)helios
#helios.bin : $(OBJ_DIR)helios.bin
#helios.efi :

#$(OBJ_DIR)helios : kernelmodules $(OBJ_DIR)link.lds
#	@echo "Linking object files."
#	@$(LD) $(OBJ_DIR)*.o -nostdlib --gc-sections -T$(OBJ_DIR)link.lds -o $(OBJ_DIR)helios
	
#$(OBJ_DIR)helios.bin : $(OBJ_DIR)helios
#	@echo "Generating kernel binary"
#	@$(CXXPREFIX)objcopy -O binary $(OBJ_DIR)helios $(OBJ_DIR)helios.bin

#kernelmodules :
#	@mkdir -p $(OBJ_DIR)
#	@for folder in $(MAKES); do $(MAKE) -C $$folder || exit; done

#$(OBJ_DIR)link.lds : $(ARCH_FOLDER)/link.lds
#	@$(CXXPREFIX)cpp $(MACROS) -xc -P -C $<  -o $@

#clean :
#	@$(RM) $(OBJ_DIR)/*.o

fclean :
	@$(RM) -rf $(OBJ_DIR)
	@$(RM) -rf $(LIB_DIR)
