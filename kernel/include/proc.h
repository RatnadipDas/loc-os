#pragma once
#include "types.h"

/**
 * @def PROCS_MAX
 * @brief Maximum number of processes the system can manage.
 */
#define PROCS_MAX 8  // Maximum number of processes

/**
 * @def PROC_UNUSED
 * @brief Indicates that the process control structure is not currently in use.
 */
#define PROC_UNUSED 0  // Unused process control structure

/**
 * @def PROC_RUNNABLE
 * @brief Indicates that the process is ready to be scheduled for execution.
 */
#define PROC_RUNNABLE 1  // Runnable process

#define PROC_EXITED 2  // Exited process

/**
 * @struct process
 * @brief Represents a process control block (PCB) in the operating system.
 *
 * This structure defines the minimal state required to manage a process.
 * It includes identifiers, execution state, memory mappings, and kernel stack.
 * The kernel uses this structure for process scheduling, context switching, and
 * memory management. Each process is represented by one instance of this struct.
 */
struct process {
    int pid;               ///< Unique process identifier assigned by the kernel.
    int state;             ///< Process state (e.g., PROC_UNUSED, PROC_RUNNABLE, etc.).
    vaddr_t sp;            ///< Saved stack pointer (virtual address) for context switching.
    uint32_t *page_table;  ///< Pointer to the root page table of the process (Sv32).
    uint8_t stack[8192];   ///< Kernel stack used during system calls and interrupts (8 KB).
};

__attribute__((naked))
/**
 * @brief Performs a context switch between two processes.
 *
 * This function saves the current process's context (callee-saved registers and stack pointer)
 * and restores the context of the next process, effectively performing a task switch.
 *
 * It is marked with `__attribute__((naked))`, meaning it does not include compiler-generated
 * prologue/epilogue code — all register saving and restoring is handled explicitly in inline
 * assembly. This is essential for low-level context switching logic in a cooperative or
 * preemptive multitasking kernel.
 *
 * @param[in,out] prev_sp Pointer to a variable where the current stack pointer (of the
 *                        outgoing process) will be saved.
 * @param[in]     next_sp Pointer to the saved stack pointer of the next process to restore.
 *
 * ### Registers Saved and Restored:
 * - `ra` (return address)
 * - `s0` to `s11` (callee-saved registers)
 *
 * ### How it Works:
 * 1. Saves `ra`, `s0`–`s11` onto the current stack.
 * 2. Saves the current `sp` (stack pointer) to `*prev_sp`.
 * 3. Loads `sp` from `*next_sp`.
 * 4. Restores `ra`, `s0`–`s11` from the new stack.
 * 5. Returns to the restored process using `ret`.
 *
 * @note Both `prev_sp` and `next_sp` must point to valid memory locations
 *       within the corresponding process structures (e.g., `proc->sp`).
 * @note This function should only be called in privileged mode with properly
 *       prepared stack frames.
 *
 * @warning Incorrect use can easily corrupt stack state and lead to undefined behavior.
 *
 * @example
 * @code
 * // In a scheduler:
 * switch_context(&current_proc->sp, &next_proc->sp);
 * @endcode
 */
void
switch_context(uint32_t *prev_sp,
               uint32_t *next_sp);

/**
 * @brief Create and initialize a new process.
 *
 * This function sets up a new user process from a raw binary image by:
 * - Locating an unused slot in the process table.
 * - Setting up an initial kernel stack frame for context switching.
 * - Allocating and initializing a new page table.
 * - Mapping both kernel memory and user memory.
 * - Mapping virtio block device memory for I/O.
 * - Copying the user program image into memory.
 * - Returning a pointer to the newly created process.
 *
 * @param image       Pointer to the binary image of the program.
 * @param image_size  Size of the binary image in bytes.
 * @param base_addr   Virtual base address where the image should be loaded.
 * @param pc          Initial program counter (entry point) for the process.
 *
 * @return Pointer to the initialized `struct process`.
 *
 * @note The created process is set to `PROC_RUNNABLE` and is ready to be scheduled.
 *       It can access the virtio block device via memory-mapped I/O.
 */
struct process *create_process(const void *image, size_t image_size, const vaddr_t base_addr, const vaddr_t pc);

/**
 * @brief Initializes the idle process.
 *
 * Creates the idle process with a reserved PID of 0 and sets it as the
 * currently running process. This process runs when no user processes are runnable.
 *
 * @note This function should be called once during system initialization.
 *
 * @code
 * // In kernel initialization code:
 * init_idle_process();
 * idle_proc = current_proc;
 * @endcode
 */
void init_idle_process(void);

/**
 * @brief Voluntarily yield the CPU to allow scheduling of another process.
 *
 * This function implements cooperative multitasking by selecting the next runnable process
 * and switching context to it. If no other process is runnable, the idle process is chosen.
 *
 * The function performs the following steps:
 * - Finds the next process in a round-robin fashion.
 * - Skips the current process unless no other option is available.
 * - Updates the `satp` CSR to switch to the new process's page table.
 * - Sets the `sscratch` CSR to point to the top of the new process's kernel stack.
 * - Performs a context switch using `switch_context`.
 *
 * Flushing the TLB before and after modifying `satp` ensures memory consistency and avoids
 * stale virtual-to-physical address translations.
 *
 * @note This should be called by a process that wants to voluntarily give up the CPU.
 *       It is essential in a cooperative multitasking environment where the kernel relies
 *       on processes to yield explicitly.
 *
 * @code
 * // Example usage within a cooperative multitasking loop:
 * while (1) {
 *     perform_computation();
 *     yield();  // Allow other processes to run
 * }
 * @endcode
 */
void yield(void);

/**
 * @brief Returns the currently running process.
 *
 * Provides access to the process control block (PCB) of the process
 * that is currently executing on the CPU.
 *
 * @return Pointer to the current `struct process`.
 *
 * @note This function is typically used by kernel subsystems that need
 *       to inspect or modify the state of the active process.
 */
struct process *get_current_process(void);
