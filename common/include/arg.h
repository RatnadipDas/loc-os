#pragma once

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
 * size_t offset = offsetof(struct Example, c); // Returns the byte offset of 'c' within 'Example'
 * printf("Offset of c: %x\n", offset);
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
 * @param param The last fixed argument before the variable arguments begin.
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
#define va_start(ap, param) __builtin_va_start(ap, param)

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
 * @note While some implementations of `va_end` may be a no-op, it is considered
 * good practice to always call it to ensure portability.
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
#define va_end(ap) __builtin_va_end(ap)

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
 * @note Each call to `va_arg` advances the argument pointer to the next argument.
 * @note The caller must ensure that `type` matches the actual argument type,
 * as mismatched types can lead to undefined behavior.
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
#define va_arg(ap, type) __builtin_va_arg(ap, type)

/**
 * @brief Copies the state of a variable argument list.
 *
 * @param dest The `va_list` variable that will receive the copy.
 * @param src The `va_list` variable to be copied.
 *
 * @note This macro is an alias for `__builtin_va_copy`, a compiler-provided
 * built-in function.
 * @note The `dest` variable must be cleaned up using `va_end` before it goes
 * out of scope to avoid memory leaks.
 * @note This is useful when a variable argument list needs to be processed
 * multiple times or preserved.
 *
 * @example
 * @code
 * void process_args(int count, ...) {
 *     va_list args, args_copy;
 *     va_start(args, count);      // Initialize the argument list
 *     va_copy(args_copy, args);   // Create a copy of the argument list
 *
 *     // Process original list
 *     for (int i = 0; i < count; i++) {
 *         printf("%d ", va_arg(args, int));
 *     }
 *     printf("\n");
 *
 *     // Process copied list
 *     for (int i = 0; i < count; i++) {
 *         printf("%d ", va_arg(args_copy, int));
 *     }
 *     printf("\n");
 *
 *     va_end(args);      // Clean up original list
 *     va_end(args_copy); // Clean up copied list
 * }
 *
 * int main() {
 *     process_args(3, 10, 20, 30);
 *     return 0;
 * }
 * @endcode
 */
#define va_copy(dest, src) __builtin_va_copy(dest, src)
