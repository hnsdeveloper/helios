MISC_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

$(BUILD_DIR)new.o: new.cpp new.hpp macros.hpp misc/types.hpp
	$(CXX) $(CPPFLAGS) $(EXTRAFLAGS) -I$(INCLUDE_DIR) -o $@ $<
