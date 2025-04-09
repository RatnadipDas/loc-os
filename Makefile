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

##########
## Disk ##
##########
DISK_NAME = disk
DISK_DIR = $(DISK_NAME)
DISK_FILE = $(BUILD_DIR)/$(KERNEL)/$(DISK_NAME).tar

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
USER_BIN_PATH = $(BUILD_DIR)/$(USER_DIR)/$(USER).bin
USER_BIN_OBJ_PATH = $(BUILD_DIR)/$(USER_DIR)/$(USER).bin.o
USER_BIN_OBJ_SYMBOLS_PATH = $(BUILD_DIR)/$(USER_DIR)/$(USER)_bin_obj_symbols.txt

##############################
## Build Directory Creation ##
##############################
$(info Creating build directory tree: "$(BUILD_DIR)" ...)
$(shell mkdir -p $(BUILD_DIR))
$(shell mkdir -p $(BUILD_DIR)/$(COMMON_DIR))
$(shell mkdir -p $(BUILD_DIR)/$(KERNEL_DIR))
$(shell mkdir -p $(BUILD_DIR)/$(USER_DIR))

###################
## Disk Creation ##
###################
$(info Creating disk file: "$(DISK_FILE)" from disk directory: "$(DISK_DIR)" ...)
$(shell cd disk && tar cf ../$(DISK_FILE) --format=ustar *.txt)

#############
## C Flags ##
#############
# -O2: Optimization level 2
# -g3: Generate debug information (level 3)
# -Wall: Enable most compiler warnings
# -Wextra: Enable extra (more aggressive) warnings
CFLAGS = -O2 -g3 -Wall -Wextra

# --target=riscv32-unknown-elf: Cross-compiling for RISC-V without an OS
# -fno-stack-protector: Disable stack protection, see: https://github.com/nuta/operating-system-in-1000-lines/issues/31#issuecomment-2613219393
# -ffreestanding: Compile in freestanding mode
# -nostdlib: Don't use the standard C library or runtime objects
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
QEMU_LOG_FILE = $(BUILD_DIR)/qemu.log

# -machine virt: Use the RISC-V "virt" virtual board
# -bios default: Use QEMU’s built-in BIOS (e.g. OpenSBI)
# -nographic: Disable graphics, use terminal only
# -serial mon:stdio: Merge monitor + serial to your terminal (standard I/O)
# --no-reboot: Don't reboot QEMU when guest exits (just halt)
QEMU_FLAGS = -machine virt -bios default -nographic -serial mon:stdio --no-reboot

# -d: Tells QEMU what to log
# unimp: Logs unimplemented device accesses or CPU instructions
# guest_errors: Logs errors generated by the guest OS, like bad memory access
# int: Logs interrupt-related information, e.g., IRQs being raised/handled
# cpu_reset: Logs CPU reset events (e.g., from software or exceptions)
# -D $(QEMU_LOG_FILE): Tells QEMU where to write the log output, typically a .log file
QEMU_FLAGS += -d unimp,guest_errors,int,cpu_reset -D $(QEMU_LOG_FILE)

#-drive: Adds a virtual drive (like a hard disk or USB device) to the system
# id=drive0: Assigns an ID (drive0) to this drive (used to refer to it elsewhere)
# file=$(DISK_FILE): Path to the disk image file (e.g. build/kernel/disk.tar)
# format=raw: Treat the file as a raw byte-for-byte disk image
# if=none: Don’t attach it to any interface automatically
QEMU_FLAGS += -drive id=drive0,file=$(DISK_FILE),format=raw,if=none

# -device: Adds a virtual hardware device to the guest system
# virtio-blk-device: The specific device type: VirtIO block device (for disk I/O)
# drive=drive0: Links this block device to the drive ID drive0 (created using -drive)
# bus=virtio-mmio-bus.0: Connects the device to the MMIO-based VirtIO bus at slot 0
QEMU_FLAGS += -device virtio-blk-device,drive=drive0,bus=virtio-mmio-bus.0
QEMU_FLAGS += -kernel

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
USER_C_OBJECTS = $(patsubst $(USER_DIR)/$(SOURCE_DIR)/%.c, $(BUILD_DIR)/$(USER_DIR)/%.o, $(USER_C_SOURCES))
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
.PHONY: all
all: build run

.PHONY: build
build: kernel-build kernel-asm user-asm kernel-dump-symbols user-dump-symbols user-bin-obj-dump-symbols

.PHONY: run
run: kernel-run

.PHONY: clean
clean:
	$(info Removing build directory tree: "$(BUILD_DIR)" ...)
	@rm -rf $(BUILD_DIR)

# Kernel Targets
.PHONY: kernel-build
kernel-build: $(KERNEL_C_OBJECTS) $(COMMON_C_OBJECTS) $(USER_BIN_OBJ_PATH)
	$(info Compiling elf file: "$(KERNEL_ELF_PATH)" from obj files: "$(strip $(KERNEL_C_OBJECTS) $(COMMON_C_OBJECTS))" ...)
	$(info And embedding user binary object file: "$(USER_BIN_OBJ_PATH)" into the elf file: "$(KERNEL_ELF_PATH)" ...)
	@$(C_COMPILER_CALL) $(KERNEL_C_OBJECTS) $(COMMON_C_OBJECTS) $(KERNEL_LDFLAGS) -o $(KERNEL_ELF_PATH) $(USER_BIN_OBJ_PATH)

.PHONY: kernel-asm
kernel-asm:
	$(info Dumping asm file: "$(KERNEL_ASM_PATH)" from elf file: "$(KERNEL_ELF_PATH)" ...)
	@$(OBJDUMP) -d $(KERNEL_ELF_PATH) > $(KERNEL_ASM_PATH)

.PHONY: kernel-dump-symbols
kernel-dump-symbols:
	$(info Dumping symbols from elf file: "$(KERNEL_ELF_PATH)" to the text file: "$(KERNEL_SYMBOLS_PATH)" ...)
	@$(NM) $(KERNEL_ELF_PATH) > $(KERNEL_SYMBOLS_PATH)

.PHONY: kernel-run
kernel-run:
	$(info Running elf file: "$(KERNEL_ELF_PATH)" on Qemu ...)
	@$(QEMU) $(QEMU_FLAGS) $(KERNEL_ELF_PATH)

# use case: make kernel-addr2line ADDRESS=xxxxxxxx
.PHONY: kernel-addr2line
kernel-addr2line:
	$(info Mapping address: "0x$(ADDRESS)" to line for elf file: "$(KERNEL_ELF_PATH)" ...)
	@$(ADDR2LINE) -e $(KERNEL_ELF_PATH) $(ADDRESS)

# User Targets
.PHONY: user-asm
user-asm:
	$(info Dumping asm file: "$(USER_ASM_PATH)" from elf file: "$(USER_ELF_PATH)" ...)
	@$(OBJDUMP) -d $(USER_ELF_PATH) > $(USER_ASM_PATH)

.PHONY: user-dump-symbols
user-dump-symbols:
	$(info Dumping symbols from elf file: "$(USER_ELF_PATH)" to the text file: "$(USER_SYMBOLS_PATH)" ...)
	@$(NM) $(USER_ELF_PATH) > $(USER_SYMBOLS_PATH)

.PHONY: user-bin-obj-symbols
user-bin-obj-dump-symbols:
	$(info Dumping symbols from bin obj file: "$(USER_BIN_OBJ_PATH)" to the text file: "$(USER_BIN_OBJ_SYMBOLS_PATH)" ...)
	@$(NM) $(USER_BIN_OBJ_PATH) > $(USER_BIN_OBJ_SYMBOLS_PATH)

# use case: make user-addr2line ADDRESS=xxxxxxxx
.PHONY: user-addr2line
user-addr2line:
	$(info Mapping address: "0x$(ADDRESS)" to line for elf file: "$(USER_ELF_PATH)" ...)
	@$(ADDR2LINE) -e $(USER_ELF_PATH) $(ADDRESS)

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

$(USER_ELF_PATH): $(USER_C_OBJECTS) $(COMMON_C_OBJECTS)
	$(info Compiling elf file: "$(USER_ELF_PATH)" from obj files: "$(strip $(USER_C_OBJECTS) $(COMMON_C_OBJECTS))" ...)
	@$(C_COMPILER_CALL) $(USER_C_OBJECTS) $(COMMON_C_OBJECTS) $(USER_LDFLAGS) -o $(USER_ELF_PATH)

$(USER_BIN_PATH): $(USER_ELF_PATH)
	$(info Creating user binary file: "$@" from elf file: "$<" ...)
	@$(OBJCOPY) --set-section-flags .bss=alloc,contents -O binary $< $@

$(USER_BIN_OBJ_PATH): $(USER_BIN_PATH)
	$(info Creating user binary object file: "$@" from binary file: "$<" ...)
	@$(OBJCOPY) -Ibinary -Oelf32-littleriscv $< $@
