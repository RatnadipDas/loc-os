#pragma once

/**
 * @brief Reads the value of a Control and Status Register (CSR).
 *
 * This macro reads the specified CSR and returns its value.
 * It uses an inline assembly statement to execute the `csrr` instruction,
 * which reads the contents of the given CSR into a register.
 *
 * @param reg The name of the CSR to read.
 * @return The value of the specified CSR.
 *
 * @note This macro is useful for accessing RISC-V control registers,
 * such as `sstatus`, `stvec`, or `scause`.
 *
 * @example
 * @code
 * unsigned long status = READ_CSR(sstatus);
 * @endcode
 */
#define READ_CSR(reg)                                         \
    ({                                                        \
        uint32_t __tmp;                                       \
        __asm__ __volatile__("csrr %0, " #reg : "=r"(__tmp)); \
        __tmp;                                                \
    })

/**
 * @brief Writes a value to a Control and Status Register (CSR).
 *
 * This macro writes the specified value to the given CSR using the
 * `csrw` instruction. It ensures that the value is first assigned to
 * a temporary variable to prevent multiple evaluations.
 *
 * @param reg The name of the CSR to write to.
 * @param value The value to be written to the CSR.
 *
 * @note This macro is useful for configuring RISC-V control registers,
 * such as `sstatus`, `stvec`, or `sie`.
 *
 * @example
 * @code
 * WRITE_CSR(stvec, (uint32_t)trampoline);
 * @endcode
 */
#define WRITE_CSR(reg, value)                                   \
    do {                                                        \
        uint32_t __tmp = (value);                               \
        __asm__ __volatile__("csrw " #reg ", %0" ::"r"(__tmp)); \
    } while (0)
