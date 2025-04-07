#include "proc.h"

#include "lib.h"
#include "utils.h"

/**
 * @brief Array of all process control structures in the system.
 *
 * This global array holds the fixed-size set of `process` structures, with a maximum
 * of `PROCS_MAX` entries. Each entry represents a distinct process in the system.
 *
 * The scheduler uses this array to track process states, assign PIDs, and perform
 * context switches between runnable processes.
 *
 * @note Unused slots have `state == PROC_UNUSED`.
 */
struct process procs[PROCS_MAX];  // All process control structures.

/**
 * @brief Pointer to the currently running process.
 *
 * This global pointer tracks the process that is actively executing on the CPU.
 * It is updated during context switches and is used by the scheduler to determine
 * which process is currently in control.
 */
struct process *current_proc;  // Currently running process

/**
 * @brief Pointer to the idle process.
 *
 * The idle process runs when no other user-defined processes are runnable.
 * It typically performs minimal work (e.g., waiting for interrupts) and ensures
 * the CPU is never idle without a task.
 */
struct process *idle_proc;  // Idle process

__attribute__((naked)) void
switch_context(uint32_t *prev_sp,
               uint32_t *next_sp) {
    __asm__ __volatile__(
        // --- Save callee-saved registers onto the current stack ---

        "addi sp, sp, -13 * 4\n"  // Allocate space on stack for 13 registers (each 4 bytes)
        "sw ra,  0  * 4(sp)\n"    // Save return address (ra)
        "sw s0,  1  * 4(sp)\n"    // Save callee-saved register s0
        "sw s1,  2  * 4(sp)\n"    // Save s1
        "sw s2,  3  * 4(sp)\n"    // Save s2
        "sw s3,  4  * 4(sp)\n"    // Save s3
        "sw s4,  5  * 4(sp)\n"    // Save s4
        "sw s5,  6  * 4(sp)\n"    // Save s5
        "sw s6,  7  * 4(sp)\n"    // Save s6
        "sw s7,  8  * 4(sp)\n"    // Save s7
        "sw s8,  9  * 4(sp)\n"    // Save s8
        "sw s9,  10 * 4(sp)\n"    // Save s9
        "sw s10, 11 * 4(sp)\n"    // Save s10
        "sw s11, 12 * 4(sp)\n"    // Save s11

        // --- Store current stack pointer to *prev_sp ---
        "sw sp, (a0)\n"  // Save the current stack pointer into *prev_sp

        // --- Load stack pointer from *next_sp ---
        "lw sp, (a1)\n"  // Load the stack pointer of the next process from *next_sp

        // --- Restore callee-saved registers from new stack ---

        "lw ra,  0  * 4(sp)\n"  // Restore return address (ra)
        "lw s0,  1  * 4(sp)\n"  // Restore s0
        "lw s1,  2  * 4(sp)\n"  // Restore s1
        "lw s2,  3  * 4(sp)\n"  // Restore s2
        "lw s3,  4  * 4(sp)\n"  // Restore s3
        "lw s4,  5  * 4(sp)\n"  // Restore s4
        "lw s5,  6  * 4(sp)\n"  // Restore s5
        "lw s6,  7  * 4(sp)\n"  // Restore s6
        "lw s7,  8  * 4(sp)\n"  // Restore s7
        "lw s8,  9  * 4(sp)\n"  // Restore s8
        "lw s9,  10 * 4(sp)\n"  // Restore s9
        "lw s10, 11 * 4(sp)\n"  // Restore s10
        "lw s11, 12 * 4(sp)\n"  // Restore s11

        // --- Adjust stack pointer after popping registers ---
        "addi sp, sp, 13 * 4\n"  // Reclaim the 13 * 4 bytes of stack used for saving registers

        // --- Return to the newly restored context ---
        "ret\n");  // Return to the instruction after previous call (restored ra)
}

struct process *create_process(uint32_t pc) {
    // Find an unused process control structure.
    struct process *proc = NULL;
    int i;
    for (i = 0; i < PROCS_MAX; i++) {
        if (procs[i].state == PROC_UNUSED) {
            proc = &procs[i];
            break;
        }
    }

    if (!proc)
        PANIC("no free process slots");

    // Stack callee-saved registers. These register values will be restored in
    // only the first context switch in switch_context.
    uint32_t *sp = (uint32_t *)&proc->stack[sizeof(proc->stack)];  // This kernel 'sp' wil only be used for the first time context switching.
                                                                   // For rest of the context switch user 'sp' will be used.
    *--sp = 0;                                                     // s11
    *--sp = 0;                                                     // s10
    *--sp = 0;                                                     // s9
    *--sp = 0;                                                     // s8
    *--sp = 0;                                                     // s7
    *--sp = 0;                                                     // s6
    *--sp = 0;                                                     // s5
    *--sp = 0;                                                     // s4
    *--sp = 0;                                                     // s3
    *--sp = 0;                                                     // s2
    *--sp = 0;                                                     // s1
    *--sp = 0;                                                     // s0
    *--sp = (uint32_t)pc;                                          // ra

    // Initialize fields.
    proc->pid = i + 1;
    proc->state = PROC_RUNNABLE;
    proc->sp = (uint32_t)sp;
    return proc;
}

void init_idle_process() {
    INFO("Initializing idle process...")
    struct process *idle_proc = create_process((uint32_t)NULL);
    idle_proc->pid = 0;
    current_proc = idle_proc;
    OK("Initialized idle process.")
};

void yield(void) {
    // Search for a runnable process
    struct process *next = idle_proc;  // Default to idle process
    for (int i = 0; i < PROCS_MAX; i++) {
        // Round-robin selection, skipping current_proc if needed
        struct process *proc = &procs[(current_proc->pid + i) % PROCS_MAX];
        if (proc->state == PROC_RUNNABLE && proc->pid > 0) {
            next = proc;
            break;
        }
    }

    // If no switch is needed, continue running current process
    if (next == current_proc)
        return;

    // Set sscratch to point to the top of the next process's kernel stack.
    // This will be used by the trap handler to restore the correct stack.
    __asm__ __volatile__(
        "csrw sscratch, %[sscratch]\n"
        :
        : [sscratch] "r"((uint32_t)&next->stack[sizeof(next->stack)]));

    // Perform context switch to the selected process
    struct process *prev = current_proc;
    current_proc = next;
    switch_context(&prev->sp, &next->sp);
}