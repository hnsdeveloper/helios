MISC_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

$(BUILD_DIR)new.o: new.cpp $(INCLUDE_DIR)misc/new.hpp $(INCLUDE_DIR)misc/macros.hpp \
  $(INCLUDE_DIR)misc/types.hpp
	$(CXX) $(CPPFLAGS) $(EXTRAFLAGS) -I$(INCLUDE_DIR) -o $@ $<
