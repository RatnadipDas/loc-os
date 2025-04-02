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

// TODO: Modify it later.
__attribute__((naked))
__attribute__((aligned(4)))
/**
 * @brief Trap handling trampoline for RISC-V.
 *
 * This function serves as the initial entry point for handling traps (exceptions and interrupts)
 * in a RISC-V system. It saves the current register state, calls `handle_trap()`, and then restores
 * the registers before returning using `sret`.
 *
 * The function performs the following steps:
 * - Saves the stack pointer (`sp`) to `sscratch`.
 * - Allocates space on the stack and saves all general-purpose registers.
 * - Calls `handle_trap()` with the current stack pointer.
 * - Restores all saved registers.
 * - Returns from the trap using `sret`.
 *
 * @note This function is marked as `naked` to prevent the compiler from generating prologue and epilogue code.
 * It is also aligned to 4 bytes to ensure proper instruction alignment.
 *
 * @warning This function must be executed in a supervisor or higher privilege mode
 * to correctly interact with the CSR registers.
 *
 * @example
 * The trap handler is typically set using:
 * @code
 * WRITE_CSR(stvec, (uint32_t)trampoline);
 * @endcode
 */
void
trampoline(void);