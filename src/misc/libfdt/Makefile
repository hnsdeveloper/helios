LIBFDT_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))


.PHONY: all

all : $(BUILD_DIR)fdt_empty_tree.o $(BUILD_DIR)fdt_overlay.o $(BUILD_DIR)fdt.o $(BUILD_DIR)fdt_strerror.o \
    $(BUILD_DIR)fdt_rw.o $(BUILD_DIR)fdt_addresses.o $(BUILD_DIR)fdt_ro.o $(BUILD_DIR)fdt_sw.o \
	$(BUILD_DIR)fdt_check.o $(BUILD_DIR)fdt_wip.o

$(BUILD_DIR)fdt_empty_tree.o: fdt_empty_tree.c libfdt_env.h $(INCLUDE_DIR)misc/types.hpp $(INCLUDE_DIR)sys/mem.hpp \
  $(INCLUDE_DIR)sys/string.hpp fdt.h libfdt.h libfdt_internal.h
	$(CXX) $(CPPFLAGS) $(EXTRAFLAGS) -I$(INCLUDE_DIR) -o $@ $<

$(BUILD_DIR)fdt_overlay.o: fdt_overlay.c libfdt_env.h $(INCLUDE_DIR)misc/types.hpp $(INCLUDE_DIR)sys/mem.hpp \
  $(INCLUDE_DIR)sys/string.hpp fdt.h libfdt.h libfdt_internal.h
	$(CXX) $(CPPFLAGS) $(EXTRAFLAGS) -I$(INCLUDE_DIR) -o $@ $<

$(BUILD_DIR)fdt.o: fdt.c libfdt_env.h $(INCLUDE_DIR)misc/types.hpp $(INCLUDE_DIR)sys/mem.hpp \
  $(INCLUDE_DIR)sys/string.hpp fdt.h libfdt.h libfdt_internal.h
	$(CXX) $(CPPFLAGS) $(EXTRAFLAGS) -I$(INCLUDE_DIR) -o $@ $<

$(BUILD_DIR)fdt_strerror.o: fdt_strerror.c libfdt_env.h $(INCLUDE_DIR)misc/types.hpp $(INCLUDE_DIR)sys/mem.hpp \
  $(INCLUDE_DIR)sys/string.hpp fdt.h libfdt.h libfdt_internal.h
	$(CXX) $(CPPFLAGS) $(EXTRAFLAGS) -I$(INCLUDE_DIR) -o $@ $<

$(BUILD_DIR)fdt_rw.o: fdt_rw.c libfdt_env.h $(INCLUDE_DIR)misc/types.hpp $(INCLUDE_DIR)sys/mem.hpp \
  $(INCLUDE_DIR)sys/string.hpp fdt.h libfdt.h libfdt_internal.h
	$(CXX) $(CPPFLAGS) $(EXTRAFLAGS) -I$(INCLUDE_DIR) -o $@ $<

$(BUILD_DIR)fdt_addresses.o: fdt_addresses.c libfdt_env.h $(INCLUDE_DIR)misc/types.hpp $(INCLUDE_DIR)sys/mem.hpp \
  $(INCLUDE_DIR)sys/string.hpp fdt.h libfdt.h libfdt_internal.h
	$(CXX) $(CPPFLAGS) $(EXTRAFLAGS) -I$(INCLUDE_DIR) -o $@ $<

$(BUILD_DIR)fdt_ro.o: fdt_ro.c libfdt_env.h $(INCLUDE_DIR)misc/types.hpp $(INCLUDE_DIR)sys/mem.hpp \
  $(INCLUDE_DIR)sys/string.hpp fdt.h libfdt.h libfdt_internal.h
	$(CXX) $(CPPFLAGS) $(EXTRAFLAGS) -I$(INCLUDE_DIR) -o $@ $<

$(BUILD_DIR)fdt_sw.o: fdt_sw.c libfdt_env.h $(INCLUDE_DIR)misc/types.hpp $(INCLUDE_DIR)sys/mem.hpp \
  $(INCLUDE_DIR)sys/string.hpp fdt.h libfdt.h libfdt_internal.h
	$(CXX) $(CPPFLAGS) $(EXTRAFLAGS) -I$(INCLUDE_DIR) -o $@ $<

$(BUILD_DIR)fdt_check.o: fdt_check.c libfdt_env.h $(INCLUDE_DIR)misc/types.hpp $(INCLUDE_DIR)sys/mem.hpp \
  $(INCLUDE_DIR)sys/string.hpp fdt.h libfdt.h libfdt_internal.h
	$(CXX) $(CPPFLAGS) $(EXTRAFLAGS) -I$(INCLUDE_DIR) -o $@ $<

$(BUILD_DIR)fdt_wip.o: fdt_wip.c libfdt_env.h $(INCLUDE_DIR)misc/types.hpp $(INCLUDE_DIR)sys/mem.hpp  \
  $(INCLUDE_DIR)sys/string.hpp fdt.h libfdt.h libfdt_internal.h
	$(CXX) $(CPPFLAGS) $(EXTRAFLAGS) -I$(INCLUDE_DIR) -o $@ $<


clean :
	$(RM) $(BUILD_DIR)fdt_check.o
	$(RM) $(BUILD_DIR)fdt_sw.o
	$(RM) $(BUILD_DIR)fdt_ro.o
	$(RM) $(BUILD_DIR)fdt_addresses.o
	$(RM) $(BUILD_DIR)fdt_rw.o
	$(RM) $(BUILD_DIR)fdt_wip.o
	$(RM) $(BUILD_DIR)fdt_strerror.o
	$(RM) $(BUILD_DIR)fdt.o
	$(RM) $(BUILD_DIR)fdt_overlay.o
	$(RM) $(BUILD_DIR)fdt_empty_tree.o
