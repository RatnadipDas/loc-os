#pragma once

/**
 * @brief Structure representing the return value of an SBI (Supervisor Binary
 * Interface) call.
 *
 * This structure is used to store the result of an SBI call, which consists of:
 * - `error`: A status code indicating success (0) or an error (negative value).
 * - `value`: A return value that varies depending on the SBI function invoked.
 *
 * @note This structure follows the RISC-V SBI specification for handling
 * supervisor calls.
 *
 * @example
 * @code
 * struct sbiret result = sbi_call(0, 0, 0, 0, 0, 0, 0, SYS_SHUTDOWN);
 * if (result.error) {
 *     printf("SBI call failed with error: %ld\n", result.error);
 * } else {
 *     printf("SBI call succeeded, value: %ld\n", result.value);
 * }
 * @endcode
 */
struct sbiret {
    long error;  ///< Error code (0 for success, negative for failure).
    long value;  ///< Return value from the SBI call.
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