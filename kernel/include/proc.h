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

/**
 * @struct process
 * @brief Represents a simple process control structure.
 *
 * This structure holds essential information about a process in the system,
 * such as its ID, execution state, and stack information. It is used by the
 * kernel to manage context switching and scheduling.
 */
struct process {
    int pid;              ///< Process ID, a unique identifier for the process.
    int state;            ///< Current state of the process (e.g., PROC_UNUSED or PROC_RUNNABLE).
    vaddr_t sp;           ///< Stack pointer (virtual address) used for context switching.
    uint8_t stack[8192];  ///< Kernel stack space allocated for the process (8 KB).
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
 * @brief Creates a new process and initializes its stack.
 *
 * This function searches for an unused process control structure from the global
 * `procs` array. If a free slot is found, it initializes the process stack with
 * default values for callee-saved registers and sets the return address to the
 * provided program counter (PC). The process is marked as runnable and a unique
 * PID is assigned.
 *
 * @param pc The program counter (entry point) where the process will start execution.
 *           If NULL, the process behaves like an idle process.
 *
 * @return Pointer to the newly created process.
 *
 * @note If no free slot is available, the system will panic with an error message.
 *
 * @code
 * // Example usage:
 * struct process *p = create_process((uint32_t)&some_function);
 * @endcode
 */
struct process *create_process(uint32_t pc);

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
 * @brief Yields the CPU to another runnable process.
 *
 * Implements cooperative multitasking by selecting a new runnable process (if any),
 * and performing a context switch to it. If no other runnable process exists, the
 * idle process is selected instead.
 *
 * This version also sets the `sscratch` CSR to point to the top of the next process's
 * kernel stack. This is useful for trap handling that relies on `sscratch` to find
 * the process stack quickly.
 *
 * @note Should be called by processes wishing to voluntarily yield the CPU.
 *
 * @code
 * // Example: called inside a running process
 * while (1) {
 *     do_work();
 *     yield();  // Give other processes a chance to run
 * }
 * @endcode
 */
void yield(void);