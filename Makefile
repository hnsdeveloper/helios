PROJECT_ROOT := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
SRC_DIR := $(PROJECT_ROOT)src/
INCLUDE_DIR := $(SRC_DIR)
BUILD_DIR := $(PROJECT_ROOT)build/

.PHONY: test

test :
	echo $(PROJECT_ROOT)

new.o: src/misc/new.cpp src/misc/new.hpp src/misc/macros.hpp \
  src/misc/types.hpp
devicetree.o: src/sys/devicetree.cpp src/sys/devicetree.hpp \
  src/arch/riscv64gc/plat_def.hpp src/misc/macros.hpp src/misc/types.hpp \
  src/mem/framemanager.hpp src/sys/bootdata.hpp src/mem/mmap.hpp \
  src/sys/cpu.hpp src/misc/libfdt/libfdt.h src/misc/libfdt/libfdt_env.h \
  src/misc/limits.hpp src/misc/concepts.hpp src/misc/typetraits.hpp \
  src/sys/mem.hpp src/sys/string.hpp src/misc/libfdt/fdt.h
bootoptions.o: src/sys/bootoptions.cpp src/sys/bootoptions.hpp \
  src/misc/leanmeanparser/optionparser.hpp src/misc/types.hpp \
  src/sys/print.hpp src/misc/limits.hpp src/misc/concepts.hpp \
  src/misc/typetraits.hpp src/sys/mem.hpp src/sys/opensbi.hpp \
  src/sys/string.hpp src/sys/cpu.hpp
panic.o: src/sys/panic.cpp src/sys/panic.hpp \
  src/arch/riscv64gc/plat_def.hpp src/misc/macros.hpp src/misc/types.hpp \
  src/sys/print.hpp src/misc/limits.hpp src/misc/concepts.hpp \
  src/misc/typetraits.hpp src/sys/mem.hpp src/sys/opensbi.hpp
cpu.o: src/sys/cpu.cpp src/sys/cpu.hpp src/misc/types.hpp \
  src/arch/riscv64gc/plat_def.hpp src/misc/macros.hpp
kmalloc.o: src/sys/kmalloc.cpp src/sys/kmalloc.hpp src/misc/macros.hpp \
  src/misc/new.hpp src/misc/types.hpp src/misc/utilities.hpp \
  src/misc/typetraits.hpp src/mem/framemanager.hpp \
  src/arch/riscv64gc/plat_def.hpp src/sys/bootdata.hpp src/sys/panic.hpp \
  src/sys/print.hpp src/misc/limits.hpp src/misc/concepts.hpp \
  src/sys/mem.hpp src/sys/opensbi.hpp
main.o: src/sys/main.cpp src/arch/riscv64gc/plat_def.hpp \
  src/misc/macros.hpp src/misc/types.hpp src/mem/framemanager.hpp \
  src/sys/bootdata.hpp src/mem/mmap.hpp src/sys/cpu.hpp \
  src/misc/githash.hpp src/misc/leanmeanparser/optionparser.hpp \
  src/misc/splash.hpp src/misc/symbols.hpp src/sys/bootoptions.hpp \
  src/sys/print.hpp src/misc/limits.hpp src/misc/concepts.hpp \
  src/misc/typetraits.hpp src/sys/mem.hpp src/sys/opensbi.hpp \
  src/sys/string.hpp src/sys/devicetree.hpp src/sys/kmalloc.hpp \
  src/misc/new.hpp src/misc/utilities.hpp
boot.o: src/arch/riscv64gc/boot/boot.cpp src/arch/riscv64gc/plat_def.hpp \
  src/misc/macros.hpp src/misc/types.hpp src/misc/symbols.hpp \
  src/sys/bootdata.hpp src/sys/mem.hpp src/sys/opensbi.hpp \
  src/sys/print.hpp src/misc/limits.hpp src/misc/concepts.hpp \
  src/misc/typetraits.hpp src/sys/string.hpp
plat_def.o: src/arch/riscv64gc/plat_def.cpp \
  src/arch/riscv64gc/plat_def.hpp src/misc/macros.hpp src/misc/types.hpp \
  src/sys/mem.hpp src/sys/opensbi.hpp src/sys/print.hpp \
  src/misc/limits.hpp src/misc/concepts.hpp src/misc/typetraits.hpp
driverframework.o: src/dev/driverframework.cpp \
  src/dev/driverframework.hpp src/misc/macros.hpp src/misc/types.hpp \
  src/sys/kmalloc.hpp src/misc/new.hpp src/misc/utilities.hpp \
  src/misc/typetraits.hpp src/ulib/double_list.hpp src/ulib/node.hpp \
  src/misc/symbols.hpp
traphandler.o: src/traphandler/traphandler.cpp \
  src/arch/riscv64gc/plat_def.hpp src/misc/macros.hpp src/misc/types.hpp \
  src/sys/print.hpp src/misc/limits.hpp src/misc/concepts.hpp \
  src/misc/typetraits.hpp src/sys/mem.hpp src/sys/opensbi.hpp

mmap.o: src/mem/mmap.cpp src/mem/mmap.hpp src/arch/riscv64gc/plat_def.hpp \
  src/misc/macros.hpp src/misc/types.hpp src/sys/bootdata.hpp \
  src/sys/cpu.hpp src/mem/framemanager.hpp src/sys/mem.hpp \
  src/sys/opensbi.hpp src/sys/print.hpp src/misc/limits.hpp \
  src/misc/concepts.hpp src/misc/typetraits.hpp
framemanager.o: src/mem/framemanager.cpp src/mem/framemanager.hpp \
  src/arch/riscv64gc/plat_def.hpp src/misc/macros.hpp src/misc/types.hpp \
  src/sys/bootdata.hpp src/mem/mmap.hpp src/sys/cpu.hpp \
  src/misc/libfdt/libfdt.h src/misc/libfdt/libfdt_env.h \
  src/misc/limits.hpp src/misc/concepts.hpp src/misc/typetraits.hpp \
  src/sys/mem.hpp src/sys/string.hpp src/misc/libfdt/fdt.h \
  src/misc/new.hpp src/sys/print.hpp src/sys/opensbi.hpp \
  src/ulib/rb_tree.hpp src/ulib/hash.hpp src/ulib/node.hpp \
  src/misc/utilities.hpp
