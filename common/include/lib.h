#pragma once
#include "types.h"

/**
 * @brief Boolean constant representing logical true.
 */
#define true 1

/**
 * @brief Boolean constant representing logical false.
 */
#define false 0

/**
 * @brief Null pointer constant.
 *
 * @note Used to represent a null pointer in pointer-related operations.
 */
#define NULL ((void *)0)

/**
 * @brief Defines the system page size in bytes.
 *
 * @note Commonly used for memory management and alignment.
 */
#define PAGE_SIZE 4096

/**
 * @brief Rounds up a given value to the nearest multiple of a specified
 * alignment.
 *
 * @param value The integer value to be aligned.
 * @param align The alignment boundary (must be a power of 2).
 * @return The smallest integer greater than or equal to `value` that is a
 * multiple of `align`.
 *
 * @note This macro utilizes `__builtin_align_up`, which requires `align` to be
 * a power of 2.
 * @note If `value` is already aligned, it remains unchanged.
 *
 * @example
 * @code
 * int aligned_value = align_up(13, 8); // Returns 16
 * int aligned_value2 = align_up(16, 8); // Returns 16 (already aligned)
 * @endcode
 */
#define align_up(value, align) __builtin_align_up(value, align)

/**
 * @brief Checks if a given value is aligned to the specified boundary.
 *
 * @param value The integer value to check.
 * @param align The alignment boundary (must be a power of 2).
 * @return A nonzero value (true) if `value` is aligned to `align`, otherwise 0
 * (false).
 *
 * @note This macro utilizes `__builtin_is_aligned`, which requires `align` to
 * be a power of 2.
 *
 * @example
 * @code
 * if (is_aligned(16, 8)) {
 *     printf("16 is aligned to 8\n"); // This will print
 * }
 * if (!is_aligned(13, 8)) {
 *     printf("13 is not aligned to 8\n"); // This will print
 * }
 * @endcode
 */
#define is_aligned(value, align) __builtin_is_aligned(value, align)

/**
 * @brief Computes the byte offset of a struct member from the beginning of the
 * struct.
 *
 * @param type The struct type.
 * @param member The name of the member within the struct.
 * @return The offset (in bytes) of `member` within `type`.
 *
 * @note This macro utilizes `__builtin_offsetof`, which is a compiler built-in
 * function.
 * @note This macro is typically used in low-level programming, such as
 * implementing generic data structures.
 *
 * @example
 * @code
 * struct Example {
 *     int a;
 *     char b;
 *     double c;
 * };
 *
 * size_t offset = offsetof(struct Example, c); // Returns the byte offset of
 * 'c' within 'Example' printf("Offset of c: %zu\n", offset);
 * @endcode
 */
#define offsetof(type, member) __builtin_offsetof(type, member)

/**
 * @brief Defines a type for handling variable argument lists.
 *
 * @note It is used in conjunction with `va_start`, `va_arg`, and `va_end` to
 * process functions with a variable number of arguments.
 *
 * @example
 * @code
 * void print_numbers(int count, ...) {
 *     va_list args;
 *     va_start(args, count);
 *     for (int i = 0; i < count; i++) {
 *         int num = va_arg(args, int);
 *         printf("%d ", num);
 *     }
 *     va_end(args);
 *     printf("\n");
 * }
 *
 * int main() {
 *     print_numbers(3, 10, 20, 30); // Output: 10 20 30
 *     return 0;
 * }
 * @endcode
 */
#define va_list __builtin_va_list

/**
 * @brief Initializes a variable argument list.
 *
 * @param ap The `va_list` variable that will be initialized.
 * @param last The last fixed argument before the variable arguments begin.
 *
 * @note This macro is an alias for `__builtin_va_start`, a compiler-provided
 * built-in function.
 * @note It must be used before retrieving arguments with `va_arg` and must be
 * followed by `va_end` to clean up.
 *
 * @example
 * @code
 * void print_numbers(int count, ...) {
 *     va_list args;
 *     va_start(args, count);  // Initialize the variable argument list
 *     for (int i = 0; i < count; i++) {
 *         int num = va_arg(args, int);
 *         printf("%d ", num);
 *     }
 *     va_end(args);  // Clean up
 *     printf("\n");
 * }
 *
 * int main() {
 *     print_numbers(3, 10, 20, 30); // Output: 10 20 30
 *     return 0;
 * }
 * @endcode
 */
#define va_start __builtin_va_start

/**
 * @brief Cleans up a variable argument list.
 *
 * @param ap The `va_list` variable that was previously initialized with
 * `va_start`.
 *
 * @note This macro is an alias for `__builtin_va_end`, a compiler-provided
 * built-in function.
 * @note It should be called after `va_start` and `va_arg` to properly clean up
 * the variable argument list.
 * @note While some implementations of `va_end` may be a no-op, it is good
 * practice to always call it.
 *
 * @example
 * @code
 * void print_numbers(int count, ...) {
 *     va_list args;
 *     va_start(args, count);  // Initialize the variable argument list
 *     for (int i = 0; i < count; i++) {
 *         int num = va_arg(args, int);
 *         printf("%d ", num);
 *     }
 *     va_end(args);  // Clean up the variable argument list
 *     printf("\n");
 * }
 *
 * int main() {
 *     print_numbers(3, 10, 20, 30); // Output: 10 20 30
 *     return 0;
 * }
 * @endcode
 */
#define va_end __builtin_va_end

/**
 * @brief Retrieves the next argument from a variable argument list.
 *
 * @param ap The `va_list` variable that was previously initialized with
 * `va_start`.
 * @param type The expected type of the next argument in the list.
 * @return The next argument in the variable argument list, cast to `type`.
 *
 * @note This macro is an alias for `__builtin_va_arg`, a compiler-provided
 * built-in function.
 * @note Each call to `va_arg` moves the argument pointer to the next argument.
 * @note The caller must ensure that the `type` matches the actual argument type
 * to avoid undefined behavior.
 *
 * @example
 * @code
 * void print_numbers(int count, ...) {
 *     va_list args;
 *     va_start(args, count);  // Initialize the variable argument list
 *     for (int i = 0; i < count; i++) {
 *         int num = va_arg(args, int);  // Retrieve next integer argument
 *         printf("%d ", num);
 *     }
 *     va_end(args);  // Clean up the variable argument list
 *     printf("\n");
 * }
 *
 * int main() {
 *     print_numbers(3, 10, 20, 30); // Output: 10 20 30
 *     return 0;
 * }
 * @endcode
 */
#define va_arg __builtin_va_arg

/**
 * @brief Fills a block of memory with a specified byte value.
 *
 * @param buf Pointer to the memory block to be filled.
 * @param value The byte value to set (converted to `unsigned char`).
 * @param size The number of bytes to fill.
 * @return A pointer to the memory block (`buf`).
 *
 * @note This function is typically used to initialize memory to a known value.
 * @note The `value` is internally converted to an `unsigned char`, meaning
 * values outside the 0-255 range will be truncated.
 *
 * @example
 * @code
 * char buffer[10];
 * memset(buffer, 0, sizeof(buffer)); // Fills the buffer with zeros
 * @endcode
 */
void *memset(void *buf, int8_t value, size_t size);

/**
 * @brief Copies a block of memory from a source location to a destination.
 *
 * @param dst Pointer to the destination memory where data will be copied.
 * @param src Pointer to the source memory from which data is copied.
 * @param size The number of bytes to copy.
 * @return A pointer to the destination memory (`dst`).
 *
 * @note The memory areas should not overlap. If overlapping is possible, use
 * `memmove` instead.
 * @note The function does not perform any boundary checking; ensure `dst` has
 * enough space.
 *
 * @example
 * @code
 * char source[] = "Hello";
 * char destination[10];
 * memcpy(destination, source, 6); // Copies "Hello\0" to destination
 * @endcode
 */
void *memcpy(void *dst, const void *src, size_t size);

/**
 * @brief Copies a null-terminated string from the source to the destination.
 *
 * @param dst Pointer to the destination buffer where the string will be copied.
 * @param src Pointer to the source null-terminated string.
 * @return A pointer to the destination string (`dst`).
 *
 * @warning The destination buffer must be large enough to hold the source
 * string, including the null terminator. No bounds checking is performed.
 * @warning If `src` and `dst` overlap, behavior is undefined.
 *
 * @note For safer copying with size limits, consider using `strncpy` instead.
 *
 * @example
 * @code
 * char source[] = "Hello";
 * char destination[10];
 * strcpy(destination, source); // Copies "Hello\0" into destination
 * @endcode
 */
char *strcpy(char *dst, const char *src);

/**
 * @brief Compares two null-terminated strings lexicographically.
 *
 * @param str1 Pointer to the first string.
 * @param str2 Pointer to the second string.
 * @return An integer indicating the relationship between the strings:
 *         - A negative value if `str1` is lexicographically less than `str2`.
 *         - Zero if `str1` and `str2` are equal.
 *         - A positive value if `str1` is lexicographically greater than
 * `str2`.
 *
 * @note The comparison is case-sensitive and is performed byte by byte.
 * @note If `str1` and `str2` have different lengths, the comparison stops at
 * the first differing character.
 *
 * @example
 * @code
 * int result = strcmp("apple", "banana"); // Returns a negative value
 * int result2 = strcmp("hello", "hello"); // Returns 0
 * int result3 = strcmp("zebra", "apple"); // Returns a positive value
 * @endcode
 */
int strcmp(const char *srt1, const char *str2);

/**
 * @brief Prints formatted output to the standard output (stdout).
 *
 * @param fmt A format string specifying how the arguments should be formatted
 * and printed.
 * @param ... Additional arguments corresponding to the format specifiers in
 * `fmt`.
 * @return The number of characters printed (excluding the null terminator) on
 * success, or a negative value if an error occurs.
 *
 * @note Supported format specifiers:
 *       - `%d` for signed integers (decimal)
 *       - `%x` for unsigned integers (hexadecimal, lowercase)
 *       - `%c` for single characters
 *       - `%s` for null-terminated strings
 *       - `%%` to print a literal `%` character
 * @note Undefined behavior may occur if the format specifiers do not match the
 * provided arguments.
 *
 * @example
 * @code
 * printf("Number: %d\n", 42);         // Prints "Number: 42"
 * printf("Hex: %x\n", 255);           // Prints "Hex: ff"
 * printf("Char: %c\n", 'A');          // Prints "Char: A"
 * printf("String: %s\n", "Hello");    // Prints "String: Hello"
 * printf("Percent: %%\n");            // Prints "Percent: %"
 * @endcode
 */
void printf(const char *fmt, ...);