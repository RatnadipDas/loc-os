#include "proc.h"

#include "alloc.h"
#include "lib.h"
#include "utils.h"
#include "vm.h"

extern char __kernel_base[], __free_ram_end[];

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

struct process *create_process(const void *image, size_t image_size, const vaddr_t base_addr, const vaddr_t pc) {
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

    // Map kernel pages.
    uint32_t *page_table = (uint32_t *)alloc_pages(1);
    for (paddr_t paddr = (paddr_t)__kernel_base; paddr < (paddr_t)__free_ram_end; paddr += PAGE_SIZE)
        map_page(page_table, paddr, paddr, PAGE_R | PAGE_W | PAGE_X);

    // Map user pages.
    for (uint32_t off = 0; off < image_size; off += PAGE_SIZE) {
        paddr_t page = alloc_pages(1);

        // Handle the case where the data to be copied is smaller than the
        // page size.
        size_t remaining = image_size - off;
        size_t copy_size = PAGE_SIZE <= remaining ? PAGE_SIZE : remaining;

        // Fill and map the page.
        memcpy((void *)page, image + off, copy_size);
        map_page(page_table, base_addr + off, page,
                 PAGE_U | PAGE_R | PAGE_W | PAGE_X);
    }

    // Initialize fields.
    proc->pid = i + 1;
    proc->state = PROC_RUNNABLE;
    proc->sp = (uint32_t)sp;
    proc->page_table = page_table;
    return proc;
}

void init_idle_process() {
    INFO("Initializing idle process...")
    struct process *idle_proc = create_process(NULL, 0, 0, (uint32_t)NULL);
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

    // satp layout for Sv32 mode:
    // | 31 - Mode (01 for Sv32) | 30-22 - ASID | 21-0 - PPN (Physical Page Number) |
    //
    // Set up the CPU for the next process:
    // 1. Flush the TLB to remove any stale virtual-to-physical address mappings.
    //    This is necessary because we are about to switch to a new page table,
    //    and the CPU might still cache old translations.
    //
    // 2. Write the new page table into the `satp` register. This effectively switches
    //    the memory space to the new process by setting the base physical page number
    //    of the root page table.
    //
    // 3. Flush the TLB again to ensure the new mappings are used immediately.
    //
    // 4. Set `sscratch` to point to the top of the new process's kernel stack.
    //    This register will be used during a trap to restore the correct stack pointer.
    __asm__ __volatile__(
        "sfence.vma\n"                  // Step 1: Invalidate old TLB entries
        "csrw satp, %[satp]\n"          // Step 2: Switch to the new page table
        "sfence.vma\n"                  // Step 3: Ensure changes take effect
        "csrw sscratch, %[sscratch]\n"  // Step 4: Set up kernel stack for trap handling
        :
        : [satp] "r"(SATP_SV32 | ((uint32_t)next->page_table / PAGE_SIZE)),
          [sscratch] "r"((uint32_t)&next->stack[sizeof(next->stack)]));

    // Perform context switch to the selected process
    struct process *prev = current_proc;
    current_proc = next;
    switch_context(&prev->sp, &next->sp);
}
