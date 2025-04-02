#include "trampoline.h"

#include "riscv.h"
#include "types.h"
#include "utils.h"

// TODO: Modify it later.
/**
 * @brief Handles unexpected traps (exceptions or interrupts).
 *
 * This function is called when a trap (exception or interrupt) occurs.
 * It retrieves relevant trap information from the `scause`, `stval`,
 * and `sepc` control and status registers (CSRs) and logs a fatal error message.
 *
 * @param f Pointer to the trap frame containing saved register states
 * before the trap occurred.
 *
 * @note This function currently does not handle traps gracefully.
 * It simply logs the trap details and halts execution using `PANIC`.
 *
 * @warning This function is not designed for normal trap handling and
 * should be modified if proper trap recovery is needed.
 *
 * @example
 * @code
 * void trap_handler() {
 *     struct trap_frame f;
 *     handle_trap(&f);
 * }
 * @endcode
 */
void handle_trap(struct trap_frame *f) {
    uint32_t scause = READ_CSR(scause);  // Stores the reason for the trap (interrupt or exception).
                                         // Helps determine if the trap was caused by a system call, page fault, illegal instruction, etc.

    uint32_t stval = READ_CSR(stval);  // Stores additional details about the trap:
                                       // If the trap was due to an illegal memory access, stval contains the faulty address.
                                       // If the trap was due to an illegal instruction, stval contains the bad instruction.

    uint32_t user_pc = READ_CSR(sepc);  // Stores the address of the instruction that caused the trap.
                                        // This is useful for resuming execution after handling an exception.

    PANIC("unexpected trap scause=0x%x, stval=0x%x, sepc=0x%x\n", scause, stval, user_pc);
}

__attribute__((naked))
// Adding __attribute__((aligned(4))) aligns the function's starting address to a 4-byte boundary.
// This is because the stvec register not only holds the address of the exception handler but also has flags representing the mode in its lower 2 bits.
// Lower 2 bits encode the trap mode:
// 00 -> Direct mode (all traps go to stvec address).
// 01 -> Vectored mode (traps go to stvec + 4 * exception_code).
__attribute__((aligned(4))) void
trampoline(void) {
    __asm__ __volatile__(
        // Save the current stack pointer to sscratch
        "csrw sscratch, sp\n"

        // Allocate space on the stack for 31 registers (each 4 bytes)
        "addi sp, sp, -4 * 31\n"

        // Save caller-saved registers (ra, gp, tp, t0-t6, a0-a7, s0-s11) to the stack
        "sw ra,  4 * 0(sp)\n"  // Save return address
        "sw gp,  4 * 1(sp)\n"  // Save global pointer
        "sw tp,  4 * 2(sp)\n"  // Save thread pointer
        "sw t0,  4 * 3(sp)\n"  // Save temporary registers
        "sw t1,  4 * 4(sp)\n"
        "sw t2,  4 * 5(sp)\n"
        "sw t3,  4 * 6(sp)\n"
        "sw t4,  4 * 7(sp)\n"
        "sw t5,  4 * 8(sp)\n"
        "sw t6,  4 * 9(sp)\n"
        "sw a0,  4 * 10(sp)\n"  // Save function arguments / return values
        "sw a1,  4 * 11(sp)\n"
        "sw a2,  4 * 12(sp)\n"
        "sw a3,  4 * 13(sp)\n"
        "sw a4,  4 * 14(sp)\n"
        "sw a5,  4 * 15(sp)\n"
        "sw a6,  4 * 16(sp)\n"
        "sw a7,  4 * 17(sp)\n"
        "sw s0,  4 * 18(sp)\n"  // Save callee-saved registers
        "sw s1,  4 * 19(sp)\n"
        "sw s2,  4 * 20(sp)\n"
        "sw s3,  4 * 21(sp)\n"
        "sw s4,  4 * 22(sp)\n"
        "sw s5,  4 * 23(sp)\n"
        "sw s6,  4 * 24(sp)\n"
        "sw s7,  4 * 25(sp)\n"
        "sw s8,  4 * 26(sp)\n"
        "sw s9,  4 * 27(sp)\n"
        "sw s10, 4 * 28(sp)\n"
        "sw s11, 4 * 29(sp)\n"

        // Store the original stack pointer saved in sscratch
        "csrr a0, sscratch\n"
        "sw a0, 4 * 30(sp)\n"

        // Pass stack pointer as an argument to handle_trap
        "mv a0, sp\n"
        "call handle_trap\n"

        // Restore registers from the stack after trap handling
        "lw ra,  4 * 0(sp)\n"
        "lw gp,  4 * 1(sp)\n"
        "lw tp,  4 * 2(sp)\n"
        "lw t0,  4 * 3(sp)\n"
        "lw t1,  4 * 4(sp)\n"
        "lw t2,  4 * 5(sp)\n"
        "lw t3,  4 * 6(sp)\n"
        "lw t4,  4 * 7(sp)\n"
        "lw t5,  4 * 8(sp)\n"
        "lw t6,  4 * 9(sp)\n"
        "lw a0,  4 * 10(sp)\n"
        "lw a1,  4 * 11(sp)\n"
        "lw a2,  4 * 12(sp)\n"
        "lw a3,  4 * 13(sp)\n"
        "lw a4,  4 * 14(sp)\n"
        "lw a5,  4 * 15(sp)\n"
        "lw a6,  4 * 16(sp)\n"
        "lw a7,  4 * 17(sp)\n"
        "lw s0,  4 * 18(sp)\n"
        "lw s1,  4 * 19(sp)\n"
        "lw s2,  4 * 20(sp)\n"
        "lw s3,  4 * 21(sp)\n"
        "lw s4,  4 * 22(sp)\n"
        "lw s5,  4 * 23(sp)\n"
        "lw s6,  4 * 24(sp)\n"
        "lw s7,  4 * 25(sp)\n"
        "lw s8,  4 * 26(sp)\n"
        "lw s9,  4 * 27(sp)\n"
        "lw s10, 4 * 28(sp)\n"
        "lw s11, 4 * 29(sp)\n"

        // Restore original stack pointer
        "lw sp,  4 * 30(sp)\n"

        // Return from trap (switch back to previous privilege mode)
        "sret\n");
}