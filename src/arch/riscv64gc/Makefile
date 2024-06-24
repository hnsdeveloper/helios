RISCV_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

.PHONY: all

all : $(BUILD_DIR)boot.o $(BUILD_DIR)plat_def.o $(BUILD_DIR)boot_asm.o $(BUILD_DIR)panic_plat.o

$(BUILD_DIR)boot.o: boot/boot.cpp plat_def.hpp $(INCLUDE_DIR)misc/macros.hpp $(INCLUDE_DIR)misc/types.hpp $(INCLUDE_DIR)misc/symbols.hpp $(INCLUDE_DIR)sys/bootdata.hpp $(INCLUDE_DIR)sys/mem.hpp $(INCLUDE_DIR)sys/opensbi.hpp \
  $(INCLUDE_DIR)sys/print.hpp $(INCLUDE_DIR)sys/string.hpp
	$(CXX) $(CPPFLAGS) $(EXTRAFLAGS) -I$(INCLUDE_DIR) -o $@ $<

$(BUILD_DIR)boot_asm.o : boot/asm.S
	$(CXX) $(CPPFLAGS) $(EXTRAFLAGS) -I$(INCLUDE_DIR) -o $@ $<
	
$(BUILD_DIR)panic_plat.o : panic/panic.S
	$(CXX) $(CPPFLAGS) $(EXTRAFLAGS) -I$(INCLUDE_DIR) -o $@ $<

$(BUILD_DIR)plat_def.o: plat_def.cpp plat_def.hpp $(INCLUDE_DIR)misc/macros.hpp $(INCLUDE_DIR)misc/types.hpp \
    $(INCLUDE_DIR)sys/mem.hpp $(INCLUDE_DIR)sys/opensbi.hpp $(INCLUDE_DIR)sys/print.hpp
	$(CXX) $(CPPFLAGS) $(EXTRAFLAGS) -I$(INCLUDE_DIR) -o $@ $<
