###################
## Makefile FLAGS ##
###################
ADDRESS ?= 00000000

####################
## File Structure ##
####################
BUILD_DIR = build
INCLUDE_DIR = include
SOURCE_DIR = src

############
## Common ##
############
COMMON = common
COMMON_DIR = $(COMMON)

############
## Kernel ##
############
KERNEL = kernel
KERNEL_DIR = $(KERNEL)
KERNEL_LINKER_SCRIPT = $(KERNEL_DIR)/$(KERNEL).ld
KERNEL_ELF_PATH = $(BUILD_DIR)/$(KERNEL_DIR)/$(KERNEL).elf
KERNEL_MAP_PATH = $(BUILD_DIR)/$(KERNEL_DIR)/$(KERNEL).map
KERNEL_ASM_PATH = $(BUILD_DIR)/$(KERNEL_DIR)/$(KERNEL).asm
KERNEL_SYMBOLS_PATH = $(BUILD_DIR)/$(KERNEL_DIR)/$(KERNEL)_symbols.txt

##########
## User ##
##########
USER = user
USER_DIR = $(USER)
USER_LINKER_SCRIPT = $(USER_DIR)/$(USER).ld
USER_ELF_PATH = $(BUILD_DIR)/$(USER_DIR)/$(USER).elf
USER_MAP_PATH = $(BUILD_DIR)/$(USER_DIR)/$(USER).map
USER_ASM_PATH = $(BUILD_DIR)/$(USER_DIR)/$(USER).asm
USER_SYMBOLS_PATH = $(BUILD_DIR)/$(USER_DIR)/$(USER)_symbols.txt

#############
## C Flags ##
#############
CFLAGS = -O2 -g3 -Wall -Wextra
CFLAGS += --target=riscv32-unknown-elf -fno-stack-protector -ffreestanding -nostdlib

########################
## C and Linker Tools ##
########################
C_STANDARD = c11
CC = /usr/local/opt/llvm/bin/clang
QEMU = /usr/local/bin/qemu-system-riscv32
OBJCOPY = /usr/local/opt/llvm/bin/llvm-objcopy
OBJDUMP = /usr/local/opt/llvm/bin/llvm-objdump
ADDR2LINE = /usr/local/opt/llvm/bin/llvm-addr2line
NM = /usr/local/opt/llvm/bin/llvm-nm

# Update C Flags
CFLAGS += -std=$(C_STANDARD)

##################
## Linker Flags ##
##################
# Kernel Linker Flags
KERNEL_LDFLAGS = -Wl,-T $(KERNEL_LINKER_SCRIPT)
KERNEL_LDFLAGS += -Wl,-Map=$(KERNEL_MAP_PATH)

# User Linker Flags
USER_LDFLAGS = -Wl,-T $(USER_LINKER_SCRIPT)
USER_LDFLAGS += -Wl,-Map=$(USER_MAP_PATH)

################
## Qemu Flags ##
################
QEMU_FLAGS = -machine virt -bios default -nographic -serial mon:stdio --no-reboot -kernel

#################################
## C Compiler and Source Files ##
#################################
C_COMPILER_CALL = $(CC) $(CFLAGS)

# Common C Sources
COMMON_C_SOURCES = $(wildcard $(COMMON_DIR)/$(SOURCE_DIR)/*.c)
COMMON_C_OBJECTS = $(patsubst $(COMMON_DIR)/$(SOURCE_DIR)/%.c, $(BUILD_DIR)/$(COMMON_DIR)/%.o, $(COMMON_C_SOURCES))
COMMON_INCLUDE_DIR = -I $(COMMON_DIR)/$(INCLUDE_DIR)

# Kernel C Sources
KERNEL_C_SOURCES = $(wildcard $(KERNEL_DIR)/$(SOURCE_DIR)/*.c)
KERNEL_C_OBJECTS = $(patsubst $(KERNEL_DIR)/$(SOURCE_DIR)/%.c, $(BUILD_DIR)/$(KERNEL_DIR)/%.o, $(KERNEL_C_SOURCES))
KERNEL_INCLUDE_DIR = -I $(KERNEL_DIR)/$(INCLUDE_DIR)

# User C Sources
USER_C_SOURCES = $(wildcard $(USER_DIR)/$(SOURCE_DIR)/*.c)
USER_C_OBJECTS = $(patsubst $(USER_DIR)/$(SOURCE_DIR)/%.c, $(BUILD_DIR)/$(KERNEL_DIR)/%.o, $(USER_C_SOURCES))
USER_INCLUDE_DIR = -I $(USER_DIR)/$(INCLUDE_DIR)

# Common Compiler Call
COMMON_C_COMPILER_CALL = $(C_COMPILER_CALL) $(COMMON_INCLUDE_DIR)

# Kernel Compiler Call
KERNEL_C_COMPILER_CALL = $(C_COMPILER_CALL) $(KERNEL_INCLUDE_DIR) $(COMMON_INCLUDE_DIR)

# User Compiler Call
USER_C_COMPILER_CALL = $(C_COMPILER_CALL) $(USER_INCLUDE_DIR) $(COMMON_INCLUDE_DIR)

##############
## Targets  ##
##############
all: create kernel-build kernel-asm kernel-dump-symbols kernel-run

create:
	$(info Creating build directory tree: "$(BUILD_DIR)" ...)
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)/$(COMMON_DIR)
	@mkdir -p $(BUILD_DIR)/$(KERNEL_DIR)
	@mkdir -p $(BUILD_DIR)/$(USER_DIR)

clean:
	$(info Removing build directory tree: "$(BUILD_DIR)" ...)
	@rm -rf $(BUILD_DIR)

# Kernel Targets
kernel-build: $(KERNEL_C_OBJECTS) $(COMMON_C_OBJECTS)
	$(info Compiling elf file: "$(KERNEL_ELF_PATH)" from object files: "$(strip $(KERNEL_C_OBJECTS) $(COMMON_C_OBJECTS))" ...)
	@$(C_COMPILER_CALL) $(KERNEL_C_OBJECTS) $(COMMON_C_OBJECTS) $(KERNEL_LDFLAGS) -o $(KERNEL_ELF_PATH)

kernel-asm:
	$(info Dumping asm file: "$(KERNEL_ASM_PATH)" from elf file: "$(KERNEL_ELF_PATH)" ...)
	@$(OBJDUMP) -d $(KERNEL_ELF_PATH) > $(KERNEL_ASM_PATH)

kernel-dump-symbols:
	$(info Dumping symbols from elf file: "$(KERNEL_ELF_PATH)" to the text file: "$(KERNEL_SYMBOLS_PATH)"...)
	@$(NM) $(KERNEL_ELF_PATH) > $(KERNEL_SYMBOLS_PATH)

kernel-run:
	$(info Running elf file: "$(KERNEL_ELF_PATH)" on Qemu ...)
	@$(QEMU) $(QEMU_FLAGS) $(KERNEL_ELF_PATH)

# use case: make kernel-addr2line ADDRESS=xxxxxxxx
kernel-addr2line:
	$(info Mapping address: "0x$(ADDRESS)" to line for elf file: "$(KERNEL_ELF_PATH)" ...)
	@$(ADDR2LINE) -e $(KERNEL_ELF_PATH) $(ADDRESS)

##############
## Patterns ##
##############
# Common Patterns
$(BUILD_DIR)/$(COMMON_DIR)%.o: $(COMMON_DIR)/$(SOURCE_DIR)/%.c
	$(info Compiling object file: "$@" from source file: "$<" ...)
	@$(COMMON_C_COMPILER_CALL) -c $< -o $@

# Kernel Patterns
$(BUILD_DIR)/$(KERNEL_DIR)%.o: $(KERNEL_DIR)/$(SOURCE_DIR)/%.c
	$(info Compiling object file: "$@" from source file: "$<" ...)
	@$(KERNEL_C_COMPILER_CALL) -c $< -o $@

# User Patterns
$(BUILD_DIR)/$(USER_DIR)%.o: $(USER_DIR)/$(SOURCE_DIR)/%.c
	$(info Compiling object file: "$@" from source file: "$<" ...)
	@$(USER_C_COMPILER_CALL) -c $< -o $@

###########
## Phony ##
###########
.PHONY: all create clean kernel-asm kernel-dump-symbols kernel-build kernel-run kernel-addr2line