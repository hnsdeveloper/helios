MEM_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))


.PHONY: all

all: $(BUILD_DIR)mmap.o $(BUILD_DIR)framemanager.o

$(BUILD_DIR)mmap.o: mmap.cpp mmap.hpp $(INCLUDE_DIR)arch/riscv64gc/plat_def.hpp \
  $(INCLUDE_DIR)misc/macros.hpp $(INCLUDE_DIR)misc/types.hpp $(INCLUDE_DIR)sys/bootdata.hpp \
  $(INCLUDE_DIR)sys/cpu.hpp framemanager.hpp $(INCLUDE_DIR)sys/mem.hpp \
  $(INCLUDE_DIR)sys/opensbi.hpp $(INCLUDE_DIR)sys/print.hpp
	@echo "Making $@"
	@$(CXX) $(CPPFLAGS) $(EXTRAFLAGS) -I$(INCLUDE_DIR) -o $@ $<

$(BUILD_DIR)framemanager.o: framemanager.cpp framemanager.hpp \
  $(INCLUDE_DIR)arch/riscv64gc/plat_def.hpp $(INCLUDE_DIR)misc/macros.hpp $(INCLUDE_DIR)misc/types.hpp \
  $(INCLUDE_DIR)sys/bootdata.hpp mmap.hpp $(INCLUDE_DIR)sys/cpu.hpp \
  $(INCLUDE_DIR)misc/libfdt/libfdt.h $(INCLUDE_DIR)misc/libfdt/libfdt_env.h \
  $(INCLUDE_DIR)sys/mem.hpp $(INCLUDE_DIR)sys/string.hpp $(INCLUDE_DIR)misc/libfdt/fdt.h \
  $(INCLUDE_DIR)misc/new.hpp $(INCLUDE_DIR)sys/print.hpp $(INCLUDE_DIR)sys/opensbi.hpp \
  $(INCLUDE_DIR)ulib/rb_tree.hpp $(INCLUDE_DIR)ulib/hash.hpp $(INCLUDE_DIR)ulib/node.hpp \
  $(INCLUDE_DIR)ulib/variant.hpp $(INCLUDE_DIR)misc/utilities.hpp
	@echo "Making $@"
	@$(CXX) $(CPPFLAGS) $(EXTRAFLAGS) -I$(INCLUDE_DIR) -o $@ $<
