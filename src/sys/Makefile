SYS_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))


.PHONY: all

all: $(BUILD_DIR)devicetree.o $(BUILD_DIR)bootoptions.o $(BUILD_DIR)panic.o $(BUILD_DIR)cpu.o $(BUILD_DIR)kmalloc.o \
  $(BUILD_DIR)main.o $(BUILD_DIR)mem.o $(BUILD_DIR)string.o $(BUILD_DIR)print.o
  
$(BUILD_DIR)devicetree.o: devicetree.cpp devicetree.hpp \
  $(INCLUDE_DIR)arch/riscv64gc/plat_def.hpp $(INCLUDE_DIR)misc/macros.hpp $(INCLUDE_DIR)misc/types.hpp \
  $(INCLUDE_DIR)mem/framemanager.hpp bootdata.hpp $(INCLUDE_DIR)mem/mmap.hpp \
  cpu.hpp $(INCLUDE_DIR)misc/libfdt/libfdt.h $(INCLUDE_DIR)misc/libfdt/libfdt_env.h \
  mem.hpp string.hpp $(INCLUDE_DIR)misc/libfdt/fdt.h
	@echo "Making $@"
	@$(CXX) $(CPPFLAGS) $(EXTRAFLAGS) -I$(INCLUDE_DIR) -o $@ $<

$(BUILD_DIR)bootoptions.o: bootoptions.cpp bootoptions.hpp \
  $(INCLUDE_DIR)misc/leanmeanparser/optionparser.hpp $(INCLUDE_DIR)misc/types.hpp \
  print.hpp mem.hpp string.hpp cpu.hpp
	@echo "Making $@"
	@$(CXX) $(CPPFLAGS) $(EXTRAFLAGS) -I$(INCLUDE_DIR) -o $@ $<

$(BUILD_DIR)panic.o: panic.cpp panic.hpp $(INCLUDE_DIR)arch/riscv64gc/plat_def.hpp $(INCLUDE_DIR)misc/macros.hpp \
 $(INCLUDE_DIR)misc/types.hpp mem.hpp 
	@echo "Making $@"
	@$(CXX) $(CPPFLAGS) $(EXTRAFLAGS) -I$(INCLUDE_DIR) -o $@ $<

$(BUILD_DIR)cpu.o: cpu.cpp cpu.hpp $(INCLUDE_DIR)misc/types.hpp $(INCLUDE_DIR)arch/riscv64gc/plat_def.hpp \
$(INCLUDE_DIR)misc/macros.hpp
	@echo "Making $@"
	@$(CXX) $(CPPFLAGS) $(EXTRAFLAGS) -I$(INCLUDE_DIR) -o $@ $<

$(BUILD_DIR)kmalloc.o: kmalloc.cpp kmalloc.hpp $(INCLUDE_DIR)misc/macros.hpp \
  $(INCLUDE_DIR)misc/new.hpp $(INCLUDE_DIR)misc/types.hpp $(INCLUDE_DIR)misc/utilities.hpp \
  $(INCLUDE_DIR)mem/framemanager.hpp $(INCLUDE_DIR)arch/riscv64gc/plat_def.hpp bootdata.hpp panic.hpp \
  print.hpp mem.hpp 
	@echo "Making $@"
	@$(CXX) $(CPPFLAGS) $(EXTRAFLAGS) -I$(INCLUDE_DIR) -o $@ $<

$(BUILD_DIR)main.o: main.cpp $(INCLUDE_DIR)arch/riscv64gc/plat_def.hpp \
  $(INCLUDE_DIR)misc/macros.hpp $(INCLUDE_DIR)misc/types.hpp $(INCLUDE_DIR)mem/framemanager.hpp \
  bootdata.hpp $(INCLUDE_DIR)mem/mmap.hpp cpu.hpp $(INCLUDE_DIR)misc/githash.hpp \
  $(INCLUDE_DIR)misc/leanmeanparser/optionparser.hpp $(INCLUDE_DIR)misc/splash.hpp $(INCLUDE_DIR)misc/symbols.hpp \
  bootoptions.hpp mem.hpp string.hpp devicetree.hpp kmalloc.hpp $(INCLUDE_DIR)misc/new.hpp \
  $(INCLUDE_DIR)misc/utilities.hpp
	@echo "Making $@"
	@$(CXX) $(CPPFLAGS) $(EXTRAFLAGS) -I$(INCLUDE_DIR) -o $@ $<
	
$(BUILD_DIR)mem.o: mem.cpp mem.hpp $(INCLUDE_DIR)misc/types.hpp $(INCLUDE_DIR)arch/riscv64gc/plat_def.hpp
	@echo "Making $@"
	@$(CXX) $(CPPFLAGS) $(EXTRAFLAGS) -I$(INCLUDE_DIR) -o $@ $<

$(BUILD_DIR)string.o: string.cpp string.hpp $(INCLUDE_DIR)misc/types.hpp
	@echo "Making $@"
	@$(CXX) $(CPPFLAGS) $(EXTRAFLAGS) -I$(INCLUDE_DIR) -o $@ $<

$(BUILD_DIR)print.o: print.cpp print.hpp $(INCLUDE_DIR)misc/utilities.hpp $(INCLUDE_DIR)misc/types.hpp $(INCLUDE_DIR)arch/riscv64gc/plat_def.hpp
	@echo "Making $@"
	@$(CXX) $(CPPFLAGS) $(EXTRAFLAGS) -I$(INCLUDE_DIR) -o $@ $<
