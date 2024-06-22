PROJECT_ROOT := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
SRC_DIR := $(PROJECT_ROOT)src/
INCLUDE_DIR := $(SRC_DIR)
BUILD_DIR := $(PROJECT_ROOT)build/
SUBDIRS := $(wildcard */.)
MAKES := $(shell find ./src -name "Makefile" | xargs dirname )
CPPFLAGS := -c -fno-omit-frame-pointer -march=rv64gc -mabi=lp64 -std=c++20  -ffreestanding -nostdlib \
-fno-exceptions -fno-rtti -mabi=lp64d -mcmodel=medany -fno-asynchronous-unwind-tables -fno-use-cxa-atexit \
-Wall -Wextra -T./src/arch/riscv64gc/link.lds

EXTRAFLAGS := -DBOOTPAGES=32 -Wall -Wextra -Werror

CXX := $(CXXPREFIX)g++
AR  := $(CXXPREFIX)ar
LD	:= $(CXXPREFIX)ld


export BUILD_DIR
export INCLUDE_DIR
export SRC_DIR
export PROJECT_ROOT
export CPPFLAGS
export EXTRAFLAGS
export CXX

.PHONY: dep

helios : dep
	$(LD) $(BUILD_DIR)*.o -nostdlib --gc-sections -Bdynamic -T./src/arch/riscv64gc/link.lds -o $(BUILD_DIR)helios
	$(CXXPREFIX)objcopy -O binary $(BUILD_DIR)helios $(BUILD_DIR)helios.bin

dep :
	@mkdir -p build
	@for folder in $(MAKES); do $(MAKE) -C $$folder || exit; done
		
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
