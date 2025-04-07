#pragma once
#include "types.h"

/**
 * @brief Represents the return status and value from an SBI (Supervisor Binary Interface) call.
 *
 * The `sbiret` structure captures the outcome of an SBI call, which enables communication
 * between the operating system and the OpenSBI firmware in RISC-V systems.
 * It contains two fields: `error` and `value`, corresponding to registers `a0` and `a1`
 * upon return from the SBI call.
 *
 * ## `error` Field:
 * - Typically contains the success or failure status of the SBI call.
 * - Standard error codes include:
 *   - `0`: Success
 *   - `-1`: Failed
 *   - `-2`: Not supported
 *   - `-3`: Invalid parameter
 *   - `-4`: Denied
 *   - `-5`: Invalid address
 *   - `-6`: Already available
 * - In some SBI calls, `error` may store a returned value instead of an error code.
 *
 * ## `value` Field:
 * - Typically holds additional return data when the SBI call succeeds.
 * - In some SBI calls, this field may not be used.
 *
 * @example
 * @code
 * struct sbiret ret = sbi_call('A', 0, 0, 0, 0, 0, 0, SYS_PUTCHAR);
 * if (ret.error >= 0) {
 *     printf("Character '%c' written successfully.\n", (char)ret.error);
 * } else {
 *     printf("SBI call failed with error code: %ld\n", ret.error);
 * }
 * @endcode
 */
struct sbiret {
    long error;  ///< Status code or returned value, depending on the SBI call.
    long value;  ///< Additional return data (may be unused in some SBI calls).
};

/**
 * @brief Outputs a single character to the console.
 *
 * This function writes the character `ch` to the standard output (typically a
 * terminal or serial console).
 *
 * @param[in] ch The character to be printed.
 *
 * @note This function is implemented using the `sbi_call` function.
 *
 * @return None.
 *
 * @example
 * @code
 * putchar('A'); // Prints 'A' to the console
 * putchar('\n'); // Prints a newline character
 * @endcode
 */
void putchar(char ch);

/**
 * @brief Reads a single character from the input using an SBI call.
 *
 * This function repeatedly calls the SBI `SYS_GETCHAR` service to retrieve a character
 * from the console input. If no character is available, it yields execution and retries.
 *
 * @note This function is implemented using the `sbi_call` function.
 *
 * @return The character read as an `int32_t`, or a negative error code if the call fails.
 *
 * @example
 * @code
 * int32_t ch = getchar();
 * if (ch >= 0) {
 *     printf("Received character: %c\n", (char)ch);
 * } else {
 *     printf("Error reading character\n");
 * }
 * @endcode
 */
int32_t getchar(void);
