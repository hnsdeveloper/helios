MISC_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

.PHONY: all

all : $(BUILD_DIR)new.o

$(BUILD_DIR)new.o: new.cpp new.hpp macros.hpp types.hpp
	@echo "Making $@"
	@$(CXX) $(CPPFLAGS) $(EXTRAFLAGS) -I$(INCLUDE_DIR) -o $@ $<
