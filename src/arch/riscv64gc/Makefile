RISCV_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

$(BUILD_DIR)boot.o: boot/boot.cpp boot/asm.s plat_def.hpp
  src/misc/macros.hpp src/misc/types.hpp src/misc/symbols.hpp \
  $(INCLUDE_DIR)sys/bootdata.hpp $(INCLUDE_DIR)sys/mem.hpp $(INCLUDE_DIR)sys/opensbi.hpp \
  $(INCLUDE_DIR)sys/print.hpp $(INCLUDE_DIR)misc/limits.hpp $(INCLUDE_DIR)misc/concepts.hpp \
  $(INCLUDE_DIR)misc/typetraits.hpp $(INCLUDE_DIR)sys/string.hpp
  	$(CXX) $(CPPFLAGS) $(EXTRAFLAGS) -I$(INCLUDE_DIR) -o $@ $<

$(BUILD_DIR)plat_def.o: plat_def.cpp plat_def.hpp $(INCLUDE_DIR)misc/macros.hpp $(INCLUDE_DIR)misc/types.hpp \
  $(INCLUDE_DIR)sys/mem.hpp $(INCLUDE_DIR)sys/opensbi.hpp $(INCLUDE_DIR)sys/print.hpp \
  $(INCLUDE_DIR)misc/limits.hpp $(INCLUDE_DIR)misc/concepts.hpp $(INCLUDE_DIR)misc/typetraits.hpp
	$(CXX) $(CPPFLAGS) $(EXTRAFLAGS) -I$(INCLUDE_DIR) -o $@ $<
