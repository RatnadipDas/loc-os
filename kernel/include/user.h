#pragma once

/**
 * @brief Base virtual address for user application images.
 *
 * This constant defines the starting virtual address at which user applications
 * are loaded in memory. It must match the entry point specified in the linker
 * script (`user.ld`), ensuring that the program counter (PC) and memory layout
 * align correctly when the process is executed.
 *
 * @note Any mismatch between this address and the linker script may cause
 * undefined behavior during process execution.
 */
#define USER_BASE 0x1000000

/**
 * @brief Supervisor User Memory access (SUM) bit in the sstatus CSR.
 *
 * When set, this bit allows supervisor-mode code to access user-mode memory.
 * This is useful during operations like copying data between user and kernel space.
 *
 * @note This bit must be set if the supervisor needs to load/store data to
 * user-space addresses. If unset, such accesses will trigger a page fault.
 *
 * @see RISC-V Privileged Spec, sstatus register
 */
#define SSTATUS_SUM (1 << 18)

/**
 * @brief Supervisor Previous Interrupt Enable (SPIE) bit in the sstatus CSR.
 *
 * This bit controls whether interrupts will be enabled when the processor
 * returns to supervisor mode from a trap (via `sret`). When set, it allows
 * interrupts to be enabled after the trap is handled.
 *
 * @note This bit must typically be set before executing `sret` to ensure that
 * interrupts are enabled in supervisor mode after returning from a trap.
 *
 * @see RISC-V Privileged Spec, sstatus register
 */
#define SSTATUS_SPIE (1 << 5)

/**
 * @brief Creates the initial user-space process.
 *
 * This function sets up a user process by loading a precompiled user binary
 * into memory. It prepares the process control block and memory mappings
 * required for execution but does not run or initialize the process itself.
 *
 * @details
 * - Loads the user binary from `_binary_build_user_user_bin_start`.
 * - Sets up the user-space memory using `USER_BASE` as the virtual base address.
 * - Passes `user_entry` as the entry point, which will be executed later when the
 *   process is scheduled and context-switched into.
 *
 * @note The created process is marked as runnable and will be scheduled
 *       by the kernel in a future context switch.
 *
 * @see create_process
 * @see user_entry
 */
void init_user(void);
