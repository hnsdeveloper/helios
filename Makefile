# Project Root
PROJECT_ROOT := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

# Directories
SRC_DIR := $(PROJECT_ROOT)src
INCLUDE_DIRS := $(PROJECT_ROOT)inc $(PROJECT_ROOT)dep $(PROJECT_ROOT)arch/$(ARCH) $(PROJECT_ROOT)dep/libfdt
ARCH_DIR := $(PROJECT_ROOT)arch/$(ARCH)
LIBFDT_DIR := $(PROJECT_ROOT)dep/libfdt
OBJ_DIR := $(PROJECT_ROOT)obj
LIB_DIR := $(PROJECT_ROOT)lib
BUILD_DIR := $(PROJECT_ROOT)build

# Source files
SRC_FILES := $(shell find $(SRC_DIR) -type f \( -name "*.c" -o -name "*.cpp" -name "*.S" -name "*.s" \))
ARCH_SRC_FILES := $(shell find $(ARCH_DIR) -type f \( -name "*.c" -o -name "*.cpp" -name "*.S" -name "*.s" \))
LIBFDT_SRC_FILES := $(shell find $(LIBFDT_DIR) -type f \( -name "*.c" -o -name "*.cpp" -name "*.S" -name "*.s" \))
LINKER_SCRIPT := $(OBJ_DIR)/link.lds

# Object files with directory structure
OBJ_FILES := $(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SRC_FILES))))
ARCH_OBJ_FILES := $(patsubst $(ARCH_DIR)/%,$(OBJ_DIR)/%,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(ARCH_SRC_FILES))))
LIBFDT_OBJ_FILES := $(patsubst $(LIBFDT_DIR)/%,$(OBJ_DIR)/%,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(LIBFDT_SRC_FILES))))
DEP_FILES := $(OBJ_FILES:.o=.d) $(ARCH_OBJ_FILES:.o=.d) $(LIBFDT_OBJ_FILES:.o=.d)

# Include architecture-specific settings
ARCH_CONFIG := $(ARCH_DIR)/config.mk
-include $(ARCH_CONFIG)

# Compiler
CXX := $(CXXPREFIX)g++
AR := $(CXXPREFIX)ar
LD := $(CXXPREFIX)ld
CPP := $(CXXPREFIX)cpp

# Compiler flags
CXXFLAGS += -c -fno-omit-frame-pointer -std=c++20 -ffreestanding -fno-exceptions -fno-rtti -fno-asynchronous-unwind-tables -fno-use-cxa-atexit -Wall -Wextra -Werror -MMD -MP $(EXTRAFLAGS)
LDFLAGS := -T $(LINKER_SCRIPT) $(EXTRAFLAGS)

# Linker script
LINKER_SCRIPT := $(ARCH_DIR)/link.lds

# Target library
LIBFDT_LIB := $(LIB_DIR)/libfdt.a

# ELF executable
TARGET := $(BUILD_DIR)/my_program

# Check for necessary variables
ifndef ARCH
    $(error ARCH is not set)
endif

ifndef CXXPREFIX
    $(error CXXPREFIX is not set)
endif

ifdef HELIOS_DEBUG
    ifeq ($(HELIOS_DEBUG), 1)
        EXTRAFLAGS := $(EXTRAFLAGS) -g
        MACROS := -DDEBUG $(MACROS)
    endif
endif

# Kernel compiling settings
BOOTPAGES := 32

# Macros settings
MACROS += -DBOOTPAGES=$(BOOTPAGES)

# Default target
all: $(TARGET) libs

# Compilation rules for source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "Compiling C file $< to $@"
	@$(CXX) $(CXXFLAGS) $(MACROS) $(patsubst %,-I%,$(INCLUDE_DIRS)) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling C++ file $< to $@"
	@$(CXX) $(CXXFLAGS) $(MACROS) $(patsubst %,-I%,$(INCLUDE_DIRS)) -c $< -o $@

$(OBJ_DIR)/%.o: $(ARCH_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "Compiling C file from ARCH $< to $@"
	@$(CXX) $(CXXFLAGS) $(MACROS) $(patsubst %,-I%,$(INCLUDE_DIRS)) -c $< -o $@

$(OBJ_DIR)/%.o: $(ARCH_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling C++ file from ARCH $< to $@"
	@$(CXX) $(CXXFLAGS) $(MACROS) $(patsubst %,-I%,$(INCLUDE_DIRS)) -c $< -o $@

$(OBJ_DIR)/%.o: $(LIBFDT_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "Compiling C file from libfdt $< to $@"
	@$(CXX) $(CXXFLAGS) $(MACROS) $(patsubst %,-I%,$(INCLUDE_DIRS)) -c $< -o $@

$(OBJ_DIR)/%.o: $(LIBFDT_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling C++ file from libfdt $< to $@"
	@$(CXX) $(CXXFLAGS) $(MACROS) $(patsubst %,-I%,$(INCLUDE_DIRS)) -c $< -o $@

# Preprocess and copy linker script
$(OBJ_DIR)/link.lds: $(LINKER_SCRIPT)
	@mkdir -p $(dir $@)
	@echo "Preprocessing linker script $<"
	@$(CPP) -E -xc $(MACROS) $< -o $@

# Linking target
$(TARGET): $(OBJ_FILES) $(ARCH_OBJ_FILES) $(LIBFDT_LIB) $(OBJ_DIR)/link.lds
	@mkdir -p $(BUILD_DIR)
	@echo "Linking $@"
	$(LD) $(OBJ_FILES) $(ARCH_OBJ_FILES) -L$(LIB_DIR) --gc-sections -nostdlib -lfdt -o $@ $(LDFLAGS)

# Libraries target
libs: $(LIBFDT_LIB)
	@echo "All libraries are built."

$(LIBFDT_LIB): $(LIBFDT_OBJ_FILES)
	@mkdir -p $(LIB_DIR)
	@echo "Archiving $@"
	@$(AR) rcs $@ $^

clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR)/*.a $(BUILD_DIR)/my_program

-include $(DEP_FILES)

.PHONY: all clean libs
