# Check for necessary variables
ifndef ARCH
    $(error ARCH is not set)
endif

ifdef HELIOS_DEBUG
    ifeq ($(HELIOS_DEBUG), 1)
        EXTRAFLAGS := $(EXTRAFLAGS) -g
        MACROS := -DDEBUG $(MACROS)
    endif
endif

# Project Root
PROJECT_ROOT := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

# Source directories
SRC_DIR := $(PROJECT_ROOT)src
ARCH_SRC_DIR := $(SRC_DIR)/arch/$(ARCH)
LIBFDT_SRC_DIR := $(PROJECT_ROOT)dep/libfdt
BOOT_SRC_DIR := $(SRC_DIR)/boot

# Include directories
ifndef (EFI_INCLUDE_PATH)
    EFI_INCLUDE_PATH := /usr/local/include/efi
endif

ifndef (USR_LOCAL_INCLUDE)
    USR_LOCAL_INCLUDE := /usr/local/include
endif

INCLUDE_DIRS := $(PROJECT_ROOT)inc $(PROJECT_ROOT)dep $(PROJECT_ROOT)dep/libfdt $(EFI_INCLUDE_PATH) $(USR_LOCAL_INCLUDE) 

# Libraries directories
ifndef (GNUEFI_LIB_DIR)
	GNUEFI_LIB_DIR := /usr/local/lib
endif

ifndef (LIBGCC_FILE)
	LIBGCC_FILE := $(shell $(CC) -print-libgcc-file-name)
endif

LIB_DIRS += $(GNUEFI_LIB_DIR) $(LIB_DIR)

# Artifacts directories
OBJ_DIR := $(PROJECT_ROOT)obj
LIB_DIR := $(PROJECT_ROOT)lib
BUILD_DIR := $(PROJECT_ROOT)build

# Source files
KERNEL_SRC_FILES := $(shell find $(SRC_DIR) \( -path $(SRC_DIR)/boot -o -path $(SRC_DIR)/arch \) -prune -o -type f \( -name "*.c" -o -name "*.cpp" -o -name "*.S" -o -name "*.s" \) -print)
ARCH_SRC_FILES := $(shell find $(ARCH_SRC_DIR) -type f \( -name "*.c" -o -name "*.cpp" -o -name "*.S" -o -name "*.s" \))
LIBFDT_SRC_FILES := $(shell find $(LIBFDT_SRC_DIR) -type f \( -name "*.c" -o -name "*.cpp" -o -name "*.S" -o -name "*.s" \))
BOOT_SRC_FILES := $(shell find $(BOOT_SRC_DIR) -type f \( -name "*.c" -o -name "*.cpp" -o -name "*.S" -o -name "*.s" \))

# Object files with directory structure
KERNEL_OBJ_FILES := $(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.S,%.o,$(KERNEL_SRC_FILES)))))
ARCH_OBJ_FILES := $(patsubst $(ARCH_SRC_DIR)/%,$(OBJ_DIR)/%,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.S,%.o,$(ARCH_SRC_FILES)))))
LIBFDT_OBJ_FILES := $(patsubst $(LIBFDT_SRC_DIR)/%,$(OBJ_DIR)/%,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.S,%.o,$(LIBFDT_SRC_FILES)))))
BOOT_OBJ_FILES := $(patsubst $(BOOT_SRC_DIR)/%,$(OBJ_DIR)/boot/%,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.S,%.o,$(BOOT_SRC_FILES)))))

# Dependency files
DEP_FILES := $(OBJ_FILES:.o=.d) $(ARCH_OBJ_FILES:.o=.d) $(LIBFDT_OBJ_FILES:.o=.d) $(BOOT_OBJ_FILES:.o=.d)

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
CXXFLAGS += -std=c++20 -c -fPIE -fno-omit-frame-pointer -ffreestanding -fno-exceptions -fno-rtti -fno-asynchronous-unwind-tables -fno-use-cxa-atexit -Wall -Wextra -Werror -MMD -MP $(EXTRAFLAGS)
CFLAGS += -c -fPIE -fno-omit-frame-pointer -ffreestanding -fno-asynchronous-unwind-tables -Wall -Wextra  -MMD -MP $(EXTRAFLAGS)
LDFLAGS := -T  $(OBJ_DIR)/kernel.lds $(EXTRAFLAGS)

BLOADERFLAGS := -Wno-error=pragmas -fPIE  -g -O2 -Wall -Wextra -Wno-pointer-sign -Werror -fno-strict-aliasing -ffreestanding -fno-stack-protector -fno-stack-check -Wno-error=maybe-uninitialized -fno-merge-all-constants -DCONFIG_riscv64 -std=c11 -D__KERNEL__ -I/usr/src/sys/build/include -c 


# Kernel compiling settings
BOOTPAGES := 32

# Kernel linker script
KERNEL_LINKER_SCRIPT := $(ARCH_SRC_DIR)/kernel.lds

# Target library
LIBFDT_LIB := $(LIB_DIR)/libfdt.a

# Kernel ELF executable
TARGET := $(BUILD_DIR)/helios

# BINARY
BINARY := $(BUILD_DIR)/helios.bin

# Macros settings
MACROS += -DBOOTPAGES=$(BOOTPAGES)

# Default target
all: $(BINARY) $(TARGET) libs

# Compilation rules for bootloader source files
$(OBJ_DIR)boot/%.o: $(BOOT_SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "Compiling C file from bootloader $< to $@"
	@$(CC) $(CFLAGS) $(MACROS) $(patsubst %,-I%,$(INCLUDE_DIRS)) -c $< -o $@

$(OBJ_DIR)boot/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling C++ file from bootloader $< to $@"
	@$(CXX) $(CXXFLAGS) $(MACROS) $(patsubst %,-I%,$(INCLUDE_DIRS)) -c $< -o $@

$(OBJ_DIR)boot/%.o: $(SRC_DIR)/%.S
	@mkdir -p $(dir $@)
	@echo "Compiling assembly file from bootloader $< to $@"
	@$(CXX) $(CXXFLAGS) $(MACROS) $(patsubst %,-I%,$(INCLUDE_DIRS)) -c $< -o $@

# Compilation rules for kernel source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "Compiling C file from kernel $< to $@"
	@$(CC) $(BLOADERFLAGS) $(MACROS) $(patsubst %,-I%,$(INCLUDE_DIRS)) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling C++ file from kernel $< to $@"
	@$(CXX) $(BLOADERFLAGS) $(MACROS) $(patsubst %,-I%,$(INCLUDE_DIRS)) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.S
	@mkdir -p $(dir $@)
	@echo "Compiling assembly file from kernel $< to $@"
	@$(CXX) $(CXXFLAGS) $(MACROS) $(patsubst %,-I%,$(INCLUDE_DIRS)) -c $< -o $@

# Compilation rules for architecture source files
$(OBJ_DIR)/%.o: $(ARCH_SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "Compiling C file from ARCH $< to $@"
	@$(CC) $(CFLAGS) $(MACROS) $(patsubst %,-I%,$(INCLUDE_DIRS)) -c $< -o $@

$(OBJ_DIR)/%.o: $(ARCH_SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling C++ file from ARCH $< to $@"
	@$(CXX) $(CXXFLAGS) $(MACROS) $(patsubst %,-I%,$(INCLUDE_DIRS)) -c $< -o $@
	
$(OBJ_DIR)/%.o: $(ARCH_SRC_DIR)/%.S
	@mkdir -p $(dir $@)
	@echo "Compiling assembly file from ARCH $< to $@"
	@$(CXX) $(CXXFLAGS) $(MACROS) $(patsubst %,-I%,$(INCLUDE_DIRS)) -c $< -o $@

# Compilation rules for Libfdt source files
$(OBJ_DIR)/%.o: $(LIBFDT_SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "Compiling C file from libfdt $< to $@"
	@$(CC) $(CXXFLAGS) $(MACROS) $(patsubst %,-I%,$(INCLUDE_DIRS)) -c $< -o $@

$(OBJ_DIR)/%.o: $(LIBFDT_SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling C++ file from libfdt $< to $@"
	@$(CXX) $(CXXFLAGS) $(MACROS) $(patsubst %,-I%,$(INCLUDE_DIRS)) -c $< -o $@

# Preprocess and copy linker script
$(OBJ_DIR)/kernel.lds: $(KERNEL_LINKER_SCRIPT)
	@mkdir -p $(dir $@)
	@echo "Preprocessing linker script $<"
	@$(CPP) -E -xc $(MACROS) $< -o $@

# Linking target
$(TARGET): $(OBJ_DIR)/kernel.lds $(KERNEL_OBJ_FILES) $(ARCH_OBJ_FILES) $(LIBFDT_LIB)
	@mkdir -p $(BUILD_DIR)
	@echo "Linking $@"
	@$(LD) $(KERNEL_OBJ_FILES) $(ARCH_OBJ_FILES) $(patsubst %, -L%, $(LIB_DIRS)) --gc-sections -nostdlib -lfdt -o $@ $(LDFLAGS)

# Binary target
$(BINARY) : $(TARGET)
	@$(OBJCOPY) -O binary $< $@

# Libraries target
libs: $(LIBFDT_LIB)

$(LIBFDT_LIB): $(LIBFDT_OBJ_FILES)
	@mkdir -p $(LIB_DIR)
	@echo "Archiving $@"
	@$(AR) rcs $@ $^


$(BUILD_DIR)/BOOTX64.efi :  $(BUILD_DIR)/bootloader 
	$(OBJCOPY) -O binary -j .text -j .sdata -j .data -j .dynamic -j .dynsym  -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc $^ $@ 

$(BUILD_DIR)/bootloader.lds : $(ARCH_SRC_DIR)/bootloader.lds
	@mkdir -p $(BUILD_DIR)
	@cp $< $@

$(BUILD_DIR)/bootloader : $(BOOT_OBJ_FILES)
	@mkdir -p $(BUILD_DIR)
	$(LD) -nostdlib --warn-common --no-undefined --fatal-warnings --build-id=sha1 -z norelro -z nocombreloc $(patsubst %, -L%, $(LIB_DIRS)) $(GNUEFI_LIB_DIR)/crt0-efi-$(ARCH)-local.o  -shared -Bsymbolic --defsym=EFI_SUBSYSTEM=0xa -T/usr/local/lib/elf_$(ARCH)_efi_local.lds $(BOOT_OBJ_FILES) -o $@ -lefi -lgnuefi $(LIBGCC_FILE)  

#riscv64-linux-gnu-ld -nostdlib  --warn-common --no-undefined --fatal-warnings --build-id=sha1 -z norelro -z nocombreloc -L/gnu-efi//apps/../riscv64/lib -L/gnu-efi//apps/../riscv64/gnuefi /gnu-efi//apps/../riscv64/gnuefi/crt0-efi-riscv64-local.o -shared -Bsymbolic -L/gnu-efi//apps/../riscv64/lib --defsym=EFI_SUBSYSTEM=0xa AllocPages.o -o AllocPages.so -T /gnu-efi//apps/../gnuefi/elf_riscv64_efi_local.lds -lefi -lgnuefi /usr/lib/gcc-cross/riscv64-linux-gnu/12/libgcc.a
bootloadertemp : $(BUILD_DIR)/BOOTX64.efi
	
clean:
	@rm -rf $(OBJ_DIR) $(LIB_DIR) $(BUILD_DIR)

-include $(DEP_FILES)

.PHONY: all helios clean libs bootloadertemp
