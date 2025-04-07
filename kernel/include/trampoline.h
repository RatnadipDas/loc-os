#pragma once
#include "types.h"

/**
 * @brief Represents the saved register state during a trap.
 *
 * This structure holds the saved values of all general-purpose registers
 * at the moment a trap (exception or interrupt) occurs. It is used to
 * restore execution context after handling the trap.
 *
 * The structure follows the RISC-V calling convention and includes:
 * - Return address (`ra`)
 * - Global pointer (`gp`)
 * - Thread pointer (`tp`)
 * - Temporary registers (`t0`-`t6`)
 * - Argument registers (`a0`-`a7`)
 * - Saved registers (`s0`-`s11`)
 * - Stack pointer (`sp`)
 *
 * @note The structure is marked with `__attribute__((packed))` to ensure
 * there is no unwanted padding between fields.
 *
 * @warning This structure must match the layout expected by the trap
 * handling mechanism. Any changes should be carefully reviewed to
 * maintain compatibility with `trampoline()` and `handle_trap()`.
 *
 * @example
 * @code
 * struct trap_frame tf;
 * save_trap_context(&tf);
 * restore_trap_context(&tf);
 * @endcode
 */
struct trap_frame {
    uint32_t ra;  /**< Return address */
    uint32_t gp;  /**< Global pointer */
    uint32_t tp;  /**< Thread pointer */
    uint32_t t0;  /**< Temporary register */
    uint32_t t1;  /**< Temporary register */
    uint32_t t2;  /**< Temporary register */
    uint32_t t3;  /**< Temporary register */
    uint32_t t4;  /**< Temporary register */
    uint32_t t5;  /**< Temporary register */
    uint32_t t6;  /**< Temporary register */
    uint32_t a0;  /**< Function argument / return value */
    uint32_t a1;  /**< Function argument */
    uint32_t a2;  /**< Function argument */
    uint32_t a3;  /**< Function argument */
    uint32_t a4;  /**< Function argument */
    uint32_t a5;  /**< Function argument */
    uint32_t a6;  /**< Function argument */
    uint32_t a7;  /**< Function argument */
    uint32_t s0;  /**< Saved register */
    uint32_t s1;  /**< Saved register */
    uint32_t s2;  /**< Saved register */
    uint32_t s3;  /**< Saved register */
    uint32_t s4;  /**< Saved register */
    uint32_t s5;  /**< Saved register */
    uint32_t s6;  /**< Saved register */
    uint32_t s7;  /**< Saved register */
    uint32_t s8;  /**< Saved register */
    uint32_t s9;  /**< Saved register */
    uint32_t s10; /**< Saved register */
    uint32_t s11; /**< Saved register */
    uint32_t sp;  /**< Stack pointer */
} __attribute__((packed));

__attribute__((naked))
__attribute__((aligned(4)))
/**
 * @brief Trap entry point for all exceptions and interrupts.
 *
 * This naked and 4-byte aligned function is used as the common entry point
 * for traps in RISC-V systems, set via the `stvec` register. It handles
 * saving all general-purpose registers onto the current kernel stack,
 * prepares the trap frame, and passes control to `handle_trap()`. After the
 * trap is handled, it restores the registers and returns to the interrupted
 * context using `sret`.
 *
 * @details
 * - The function uses the `sscratch` register to retrieve and temporarily store
 *   the current kernel stack pointer.
 * - It saves 31 general-purpose registers (including all caller- and
 *   callee-saved) onto the stack.
 * - The original `sp` is saved at slot 30 in the trap frame.
 * - The address of `trampoline` must be aligned to 4 bytes so the low 2 bits
 *   of `stvec` can be used to select direct or vectored mode.
 * - After trap handling, registers and `sp` are restored before returning
 *   using `sret`, which switches back to the previous privilege mode.
 *
 * @note This function should be installed in the `stvec` register in direct mode:
 * @code
 *     // Example: set stvec to trampoline
 *     WRITE_CSR(stvec, (uint32_t)trampoline);
 * @endcode
 */
void
trampoline(void);
