MAKEFLAGS += --no-builtin-rules

# Project Root
PROJECT_ROOT := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

# Source directories
SRC_DIR := $(PROJECT_ROOT)src
LIBFDT_SRC_DIR := $(PROJECT_ROOT)dep/libfdt

# Include directories

ifndef (EFI_INCLUDE_PATH)
	EFI_INCLUDE_PATH := /usr/local/include/efi /usr/local/include
endif

INCLUDE_DIRS += $(PROJECT_ROOT)inc $(LIBFDT_SRC_DIR) $(EFI_INCLUDE_PATH)


# Artifacts directories
OBJ_DIR := $(PROJECT_ROOT)obj
LIB_DIR := $(PROJECT_ROOT)lib
BUILD_DIR := $(PROJECT_ROOT)build

# Source files
BOOT_ARCH_FILES := $(shell find $(SRC_DIR)/arch/$(ARCH) -type f \( -name "$(ARCH)reloc.c" -o -name "$(ARCH)crt0.S" \))
BOOT_SRC_FILES := $(shell find $(SRC_DIR)/boot -type f \( -name "*.c" -o -name "*.cpp" -o -name "*.S" -o -name "*.s" \))
ARCH_SRC_FILES := $(shell find $(SRC_DIR)/arch/$(ARCH) -type f \( -name "*.c" -o -name "*.cpp" -o -name "*.S" -o -name "*.s" \) $(foreach file, $(BOOT_ARCH_FILES), ! -name $(notdir $(file))))
KLIBC_SRC_FILES := $(shell find $(SRC_DIR)/klibc -type f \( -name "*.c" -o -name "*.cpp" -o -name "*.S" -o -name "*.s" \))
MEM_MODULE_SRC_FILES := $(shell find $(SRC_DIR)/mem -type f \( -name "*.c" -o -name "*.cpp" -o -name "*.S" -o -name "*.s" \))
EXTRA_MODULE_SRC_FILES := $(shell find $(SRC_DIR)/misc -type f \( -name "*.c" -o -name "*.cpp" -o -name "*.S" -o -name "*.s" \))
SYS_MODULE_SRC_FILES := $(shell find $(SRC_DIR)/sys -type f \( -name "*.c" -o -name "*.cpp" -o -name "*.S" -o -name "*.s" \))
ULIB_MODULE_SRC_FILES := $(shell find $(SRC_DIR)/ulib -type f \( -name "*.c" -o -name "*.cpp" -o -name "*.S" -o -name "*.s" \))
LIBFDT_SRC_FILES := $(shell find $(LIBFDT_SRC_DIR) -type f \( -name "*.c" -o -name "*.cpp" -o -name "*.S" -o -name "*.s" \))

# Object files with directory structure
BOOT_ARCH_OBJ_FILES := $(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.S,%.o,$(BOOT_ARCH_FILES)))))
BOOT_OBJ_FILES := $(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.S,%.o,$(BOOT_SRC_FILES)))))
ARCH_OBJ_FILES := $(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.S,%.o,$(ARCH_SRC_FILES)))))
KLIBC_OBJ_FILES := $(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.S,%.o,$(KLIBC_SRC_FILES)))))
MEM_MODULE_OBJ_FILES := $(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.S,%.o,$(MEM_MODULE_SRC_FILES)))))
EXTRA_MODULE_OBJ_FILES := $(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.S,%.o,$(EXTRA_MODULE_SRC_FILES)))))
SYS_MODULE_OBJ_FILES := $(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.S,%.o,$(SYS_MODULE_SRC_FILES)))))
ULIB_MODULE_OBJ_FILES := $(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.S,%.o,$(ULIB_MODULE_SRC_FILES)))))
LIBFDT_OBJ_FILES := $(patsubst $(LIBFDT_SRC_DIR)/%,$(OBJ_DIR)/%,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.S,%.o,$(LIBFDT_SRC_FILES)))))

# Objects needed by each target
BOOTLOADER_OBJ_FILES := $(BOOT_ARCH_OBJ_FILES) $(BOOT_OBJ_FILES) $(KLIBC_OBJ_FILES)
KERNEL_OBJ_FILES := $(ARCH_OBJ_FILES) $(KLIBC_OBJ_FILES) $(MEM_MODULE_OBJ_FILES) $(EXTRA_MODULE_OBJ_FILES) $(SYS_MODULE_OBJ_FILES) $(ULIB_MODULE_OBJ_FILES)

# Dependency files
DEP_FILES := $(KERNEL_OBJ_FILES:.o=.d) $(ARCH_OBJ_FILES:.o=.d) $(BOOT_OBJ_FILES:.o=.d) $(LIBFDT_OBJ_FILES:.o=.d)

# Include architecture-specific settings
ARCH_CONFIG := $(ARCH_SRC_DIR)/config.mk
-include $(ARCH_CONFIG)

# Compiler
CC := $(CXXPREFIX)$(CROSS_COMPILE)gcc
CXX := $(CXXPREFIX)$(CROSS_COMPILE)g++
AR := $(CXXPREFIX)$(CROSS_COMPILE)ar
OBJCOPY := $(CXXPREFIX)$(CROSS_COMPILE)objcopy
LD := $(CXXPREFIX)$(CROSS_COMPILE)ld
CPP := $(CXXPREFIX)$(CROSS_COMPILE)cpp

# Compiler flags
CPPFLAGS += -std=c++20 -fno-exceptions -fno-rtti -fno-use-cxa-atexit
CFLAGS += -std=c11
DISABLED_ERRORS := -Wno-error=unused-parameter
EXTRAFLAGS := $(EXTRAFLAGS) -c -fno-omit-frame-pointer -ffreestanding -fno-stack-protector -fno-stack-check -fno-strict-aliasing -fshort-wchar -fno-lto -fno-asynchronous-unwind-tables -fPIE -Wall -Wextra -Werror -MMD -MP $(DISABLED_ERRORS)

ifdef (VERBOSE_CC,)
    EXTRAFLAGS := $(EXTRAFLAGS) --verbose
endif


# Kernel compiling settings
BOOTPAGES := 32

# Kernel linker script
KERNEL_LINKER_SCRIPT := $(SRC_DIR)/arch/$(ARCH)/kernel.lds

# Target library
LIBFDT := $(LIB_DIR)/libfdt.a

# Kernel ELF executable
TARGET := $(BUILD_DIR)/helios

# BINARY
BINARY := $(BUILD_DIR)/helios.bin

# Macros settings
MACROS += -DBOOTPAGES=$(BOOTPAGES)

# Default target
all: $(BINARY) $(TARGET) libs


# Compilation rules for bootloader source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "Compiling C file from $< to $@"
	@$(CC) $(CFLAGS) $(EXTRAFLAGS) $(MACROS) $(patsubst %,-I%,$(INCLUDE_DIRS)) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling C++ file from $< to $@"
	@$(CXX) $(CPPFLAGS) $(EXTRAFLAGS) $(MACROS) $(patsubst %,-I%,$(INCLUDE_DIRS)) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.S
	@mkdir -p $(dir $@)
	@echo "Compiling assembly file from $< to $@"
	@$(CXX) $(CPPFLAGS) $(EXTRAFLAGS) $(MACROS) $(patsubst %,-I%,$(INCLUDE_DIRS)) -c $< -o $@

# Compilation rules for Libfdt source files
$(OBJ_DIR)/%.o: $(LIBFDT_SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "Compiling C file from libfdt $< to $@"
	$(CC) $(CFLAGS) $(EXTRAFLAGS) $(MACROS) $(patsubst %,-I%,$(INCLUDE_DIRS)) -c $< -o $@

$(OBJ_DIR)/%.o: $(LIBFDT_SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	echo $(CPATH)
	@echo "Compiling C++ file from libfdt $< to $@"
	@$(CXX) $(CPPFLAGS) $(EXTRAFLAGS) $(MACROS) $(patsubst %,-I%,$(INCLUDE_DIRS)) -c $< -o $@

# Preprocess and copy linker script
$(OBJ_DIR)/kernel.lds: $(KERNEL_LINKER_SCRIPT)
	@mkdir -p $(dir $@)
	@echo "Preprocessing linker script $<"
	@$(CPP) -E -xc $(MACROS) $< -o $@

# Linking target
$(TARGET): $(OBJ_DIR)/kernel.lds $(KERNEL_OBJ_FILES) $(LIBFDT)
	@mkdir -p $(BUILD_DIR)
	@echo "Linking $@"
	@$(LD) $(KERNEL_OBJ_FILES) $(ARCH_OBJ_FILES) $(patsubst %, -L%, $(LIB_DIRS)) --gc-sections -nostdlib -lfdt -T$(OBJ_DIR)/kernel.lds $(LDFLAGS) -o $@ 

# Binary target
$(BINARY): $(TARGET)
	@$(OBJCOPY) -O binary $< $@

# Bootloader target
bootloader: $(BUILD_DIR)/BOOTX64.efi

$(BUILD_DIR)/BOOTX64.efi: $(BUILD_DIR)/bootloader
	@mkdir -p $(BUILD_DIR)
	@$(OBJCOPY) -O binary $< $@
	@dd if=/dev/zero of=$@ bs=4096 count=0 seek=$$(( ($$(wc -c < $@) + 4095) / 4096 ))

$(BUILD_DIR)/bootloader: $(BOOT_OBJ_FILES) $(BOOT_ARCH_OBJ_FILES) $(KLIBC_OBJ_FILES) $(ARCH_SRC_DIR)/$(ARCH)bootloader.lds
	@mkdir -p $(BUILD_DIR)
	@$(LD) $(BOOT_ARCH_OBJ_FILES) $(BOOT_OBJ_FILES) $(LDFLAGS) $(patsubst %, -L%, $(LIB_DIRS)) -T$(ARCH_SRC_DIR)/$(ARCH)bootloader.lds -nostdlib -pie -z text -z max-page-size=0x1000 -o $@

# Libraries target
libs: libfdt

libfdt: $(LIBFDT)

$(LIBFDT): $(LIBFDT_OBJ_FILES)
	@mkdir -p $(LIB_DIR)
	@echo "Archiving $@"
	@$(AR) rcs $@ $^

show :
	@echo $(KERNEL_OBJ_FILES)
	@echo $(ARCH_OBJ_FILES)
	@echo $(BOOT_OBJ_FILES)
	@echo $(BOOT_ARCH_OBJ_FILES)
	@echo $(LIBFDT_SRC_FILES)

# Clean target
clean:
	@rm -rf $(OBJ_DIR) $(LIB_DIR) $(BUILD_DIR)

-include $(DEP_FILES)

.PHONY: all helios clean libs libfdt bootloader show
