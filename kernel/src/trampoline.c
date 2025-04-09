#include "trampoline.h"

#include "fs.h"
#include "proc.h"
#include "riscv.h"
#include "sbi.h"
#include "sys.h"
#include "types.h"
#include "utils.h"

/**
 * @brief Handles system calls made by user processes.
 *
 * This function dispatches system calls based on the value in register `a3` of the provided
 * trap frame. The following system calls are supported:
 *
 * - `SYS_PUTCHAR`: Writes a character (from `a0`) to the console.
 * - `SYS_GETCHAR`: Reads a character from the console into `a0`.
 * - `SYS_EXIT`: Marks the current process as exited and yields the CPU.
 * - `SYS_READFILE`: Reads data from a file specified by `a0` into a buffer at `a1`.
 * - `SYS_WRITEFILE`: Writes data from a buffer at `a1` to a file specified by `a0`.
 * - `SYS_SHUTDOWN`: Initiates system shutdown.
 *
 * For `SYS_READFILE` and `SYS_WRITEFILE`:
 * - `a0`: const char* (filename)
 * - `a1`: char* (buffer)
 * - `a2`: int32_t (length to read/write)
 *
 * @param f Pointer to the trap frame containing syscall arguments and return values.
 *
 * @note The function will panic if an unrecognized syscall number is encountered.
 */
void handle_syscall(struct trap_frame *f) {
    switch (f->a3) {
        case SYS_PUTCHAR:
            putchar(f->a0);
            break;
        case SYS_GETCHAR:
            f->a0 = getchar();
            break;
        case SYS_EXIT:
            struct process *current_process = get_current_process();
            INFO("process %d exited", current_process->pid);
            current_process->state = PROC_EXITED;
            yield();
            PANIC("unreachable");
            break;
        case SYS_READFILE:
        case SYS_WRITEFILE:
            const char *filename = (const char *)f->a0;
            char *buf = (char *)f->a1;
            int32_t len = f->a2;
            struct file *file = fs_lookup(filename);
            if (!file) {
                FAILED("file not found: %s\n", filename);
                f->a0 = -1;
                break;
            }

            if (len > (int32_t)sizeof(file->data)) {
                if (f->a3 == SYS_WRITEFILE)
                    len = (int32_t)sizeof(file->data);
                else
                    len = file->size;
            }

            if (f->a3 == SYS_WRITEFILE) {
                memcpy(file->data, buf, len);
                file->size = len;
                flush_fs();
            } else {
                memcpy(buf, file->data, len);
            }

            f->a0 = len;
            break;
        case SYS_SHUTDOWN:
            INFO("Shuting down...");
            shutdown();
            break;
        default:
            PANIC("unexpected syscall a3=0x%x\n", f->a3);
    }
}

/**
 * @brief Trap handler for both exceptions and interrupts.
 *
 * This function is invoked whenever a trap occurs in the system. A trap can be either
 * an **interrupt** (asynchronous event) or an **exception** (synchronous event).
 * The cause of the trap is identified using the `scause` CSR, while `stval` and `sepc`
 * provide additional context like the faulting address or instruction and program counter
 * at the time of trap respectively.
 *
 * If the trap is an environment call (`ECALL`), it is handled via `handle_syscall()`.
 * All other traps cause a system panic with diagnostic information.
 *
 * ---
 *
 * ### scause format
 * - Bit 31: Indicates the type of trap.
 *   - `0`: Exception
 *   - `1`: Interrupt
 * - Bits 30:0: Trap cause code
 *
 * ---
 *
 * ### Trap Cause Reference
 *
 * #### 1. Interrupts (`scause[31] = 1`)
 * | Code | Interrupt Type                        |
 * |:----:|:--------------------------------------|
 * |  1   | Supervisor software interrupt (SSIP)  |
 * |  5   | Supervisor timer interrupt (STIP)     |
 * |  9   | Supervisor external interrupt (SEIP)  |
 *
 * #### 2. Exceptions (`scause[31] = 0`)
 * | Code | Exception Type                                            |
 * |:----:|:----------------------------------------------------------|
 * |  0   | Instruction address misaligned                            |
 * |  1   | Instruction access fault                                  |
 * |  2   | Illegal instruction                                       |
 * |  3   | Breakpoint (`ebreak` instruction)                         |
 * |  4   | Load address misaligned                                   |
 * |  5   | Load access fault                                         |
 * |  6   | Store/AMO address misaligned                              |
 * |  7   | Store/AMO access fault                                    |
 * |  8   | Environment call from user mode (`ECALL` in U-mode)       |
 * |  9   | Environment call from supervisor mode (`ECALL` in S-mode) |
 * | 12   | Instruction page fault                                    |
 * | 13   | Load page fault                                           |
 * | 14   | Store/AMO page fault                                      |
 *
 * ---
 *
 * @param f Pointer to the current trap frame (register snapshot).
 */
void handle_trap(struct trap_frame *f) {
    uint32_t scause = READ_CSR(scause);  // Stores the reason for the trap (interrupt or exception).
                                         // Helps determine if the trap was caused by a system call, page fault, illegal instruction, etc.

    uint32_t stval = READ_CSR(stval);  // Stores additional details about the trap:
                                       // If the trap was due to an illegal memory access, stval contains the faulty address.
                                       // If the trap was due to an illegal instruction, stval contains the bad instruction.

    uint32_t user_pc = READ_CSR(sepc);  // Stores the address of the instruction that caused the trap.
                                        // This is useful for resuming execution after handling an exception.

    if (scause == SCAUSE_ECALL) {
        handle_syscall(f);
        user_pc += 4;  // moves the program counter forward to skip the ecall instruction.
                       //  Else trap will be executed endlessly
                       //  In RV32I, RV64I, and RV128I, all instructions are 32-bit (4 bytes).
    } else {
        PANIC("unexpected trap scause=0x%x, stval=0x%x, sepc=0x%x\n", scause, stval, user_pc);
    }

    WRITE_CSR(sepc, user_pc);  // Resume execution from updated PC
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
        // Retrieve the kernel stack of the running process from sscratch.
        "csrrw sp, sscratch, sp\n"

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

        // Retrieve and save the sp at the time of exception.
        "csrr a0, sscratch\n"
        "sw a0,  4 * 30(sp)\n"

        // Reset the kernel stack.
        "addi a0, sp, 4 * 31\n"
        "csrw sscratch, a0\n"

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
