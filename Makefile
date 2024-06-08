PROJECT_ROOT := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
SRC_DIR := $(PROJECT_ROOT)src/
INCLUDE_DIR := $(SRC_DIR)
BUILD_DIR := $(PROJECT_ROOT)build/
SUBDIRS := $(wildcard */.)

export BUILD_DIR
export INCLUDE_DIR
export SRC_DIR
export PROJECT_ROOT
export CPPFLAGS
export EXTRAFLAGS
export CXX

.PHONY: test

$(BUILD_DIR)helios : $(BUILD_DIR)boot.o $(BUILD_DIR)plat_def.o $(BUILD_DIR)mmap.o $(BUILD_DIR)framemanager.o \
	$(BUILD_DIR)fdt_empty_tree.o $(BUILD_DIR)fdt_overlay.o $(BUILD_DIR)fdt.o $(BUILD_DIR)fdt_strerror.o \
	$(BUILD_DIR)fdt_wip.o $(BUILD_DIR)fdt_rw.o $(BUILD_DIR)fdt_addresses.o $(BUILD_DIR)fdt_ro.o $(BUILD_DIR)fdt_sw.o \
	$(BUILD_DIR)fdt_check.o $(BUILD_DIR)new.o $(BUILD_DIR)devicetree.o $(BUILD_DIR)bootoptions.o $(BUILD_DIR)panic.o \
	$(BUILD_DIR)cpu.o $(BUILD_DIR)kmalloc.o  $(BUILD_DIR)main.o



  
#driverframework.o: src/dev/driverframework.cpp \
  src/dev/driverframework.hpp src/misc/macros.hpp src/misc/types.hpp \
  src/sys/kmalloc.hpp src/misc/new.hpp src/misc/utilities.hpp \
  src/misc/typetraits.hpp src/ulib/double_list.hpp src/ulib/node.hpp \
  src/misc/symbols.hpp
#traphandler.o: src/traphandler/traphandler.cpp \
  src/arch/riscv64gc/plat_def.hpp src/misc/macros.hpp src/misc/types.hpp \
  src/sys/print.hpp src/misc/limits.hpp src/misc/concepts.hpp \
  src/misc/typetraits.hpp src/sys/mem.hpp src/sys/opensbi.hpp
