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
 * @brief Fills a block of memory with a specified byte value.
 *
 * This function sets the first `n` bytes of the memory block pointed to by `buf`
 * to the specified byte value `c`.
 *
 * @param buf Pointer to the memory block to be filled.
 * @param c Value to be set (cast to `uint8_t` to ensure proper behavior).
 * @param n Number of bytes to set.
 * @return void* Pointer to the memory block `buf`.
 *
 * @note This function is typically used to initialize memory to a known value.
 * @note The function does not check for buffer overflow. Ensure that `buf`
 *       has at least `n` bytes allocated.
 * @note The `c` is internally converted to an `uint8_t`, meaning
 * values outside the 0-255 range will be truncated.
 *
 * @example
 * @code
 * char buffer[10];
 * memset(buffer, 0, sizeof(buffer)); // Fills the buffer with zeros
 * @endcode
 */
void *memset(void *buf, int8_t c, size_t n);

/**
 * @brief Copies a block of memory from a source location to a destination.
 *
 * @param dst Pointer to the destination memory where data will be copied.
 * @param src Pointer to the source memory from which data is copied.
 * @param n The number of bytes to copy.
 * @return A pointer to the destination memory (`dst`).
 *
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
void *memcpy(void *dst, const void *src, size_t n);

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
int strcmp(const char *str1, const char *str2);

/**
 * @brief Computes the length of a null-terminated string.
 *
 * This function iterates through the string until it encounters
 * the null terminator (`'\0'`), counting the number of characters.
 *
 * @param str Pointer to the input string.
 * @return The number of characters in the string before the null terminator.
 *
 * @note The function does not include the null terminator in the count.
 *
 * @example
 * @code
 * printf("%d", strlen("hello")); // Output: 5
 * @endcode
 */
size_t strlen(char *str);

/**
 * @brief Reverses a null-terminated string in place.
 *
 * This function swaps characters from the beginning and end of the string
 * until it reaches the middle, effectively reversing the string.
 *
 * @param str Pointer to the string to be reversed.
 * @return The same pointer to the reversed string.
 *
 * @note The function modifies the original string and does not allocate memory.
 *
 * @example
 * @code
 * char s[] = "hello";
 * printf("%s", strrev(s)); // Output: "olleh"
 * @endcode
 */
char *strrev(char *str);

/**
 * @brief Appends one string to another.
 *
 * This function concatenates the source string (`src`) to the destination string (`dest`).
 * The `dest` string must have enough space to accommodate the additional characters
 * from `src`, including the null terminator.
 *
 * @param dest Pointer to the destination string, which must be large enough to hold the result.
 * @param src Pointer to the source string, which is appended to `dest`.
 * @return A pointer to the destination string (`dest`).
 *
 * @example
 * @code
 * char str1[20] = "Hello, ";
 * char str2[] = "World!";
 * strcat(str1, str2);
 * printf("%s\n", str1); // Output: "Hello, World!"
 * @endcode
 */
char *strcat(char *dest, const char *src);

/**
 * @brief Converts an integer to a string representation in a given base.
 *
 * This function converts a signed 32-bit integer to a null-terminated
 * string in the specified base (2 to 36). The result is stored in `str`.
 *
 * @param num The integer to convert.
 * @param str Pointer to the buffer where the resulting string will be stored.
 * @param base The numeric base (2 to 36) for conversion.
 * @return A pointer to the resulting null-terminated string.
 *
 * @note The buffer `str` must be large enough to hold the resulting string,
 *       including the null terminator.
 * @note Only base 10 supports negative numbers; for other bases,
 *       the number is treated as unsigned.
 *
 * @example
 * @code
 * int main() {
 *     char buffer[20];
 *     printf("%s\n", itoa(1234, buffer, 10));  // Output: "1234"
 *     printf("%s\n", itoa(-1234, buffer, 10)); // Output: "-1234"
 *     printf("%s\n", itoa(255, buffer, 16));   // Output: "ff"
 *     printf("%s\n", itoa(255, buffer, 2));    // Output: "11111111"
 *     return 0;
 * }
 * @endcode
 */
char *itoa(int32_t num, char *str, size_t base);

/**
 * @brief Converts a string to an integer.
 *
 * This function converts a numeric string (in various bases) into an `int32_t` value.
 * It supports decimal, binary, octal, and hexadecimal number representations. The
 * function also handles negative numbers and ignores leading whitespace and zeros.
 *
 * ## Supported Number Formats:
 * - Decimal (default): `"123"` → `123`
 * - Binary (prefix '0b'): `"0b110"` → `6`
 * - Octal (prefix '0o'): `"0o17"` → `15`
 * - Hexadecimal (prefix '0x'): `"x1f"` → `31`
 * - Negative decimal: `"-42"` → `-42`
 * - Negative Binary: `"-0b110"` → `-6`
 * - Negative octal: `"-0o17"` → `-15`
 * - Negative hexadecimal: `"-0x1f"` → `-31`
 *
 * @param str Pointer to a null-terminated string representing a number.
 * @return The converted integer value.
 *
 * @note Assumes valid input; no error checking for malformed strings.
 *
 * @example
 * @code
 * printf("%d\n", atoi("123"));      // Output: 123
 * printf("%d\n", atoi("-123"));     // Output: -123
 * printf("%d\n", atoi("0b1010"));   // Output: 10
 * printf("%d\n", atoi("-0b1010"));  // Output: -10
 * printf("%d\n", atoi("0o10"));     // Output: 8
 * printf("%d\n", atoi("-0o10"));    // Output: -8
 * printf("%d\n", atoi("0xff"));     // Output: 255
 * printf("%d\n", atoi("-0xff"));    // Output: -255
 * @endcode
 */
int32_t atoi(const char *str);

/**
 * @brief Prints formatted output to the standard output.
 *
 * This function is a simplified implementation of `printf` that supports
 * format specifiers for different data types. The function processes the
 * format string (`fmt`), replacing format specifiers with corresponding
 * argument values.
 *
 * ## Supported Format Specifiers:
 * - `%c` → Character
 * - `%s` → Null-terminated string
 * - `%d` → Signed decimal integer (`int32_t`)
 * - `%b` → Binary representation (`uint32_t`)
 * - `%o` → Octal representation (`uint32_t`)
 * - `%x` → Hexadecimal representation (`uint32_t`)
 * - `%%` → Prints a literal `%` character
 *
 * @param fmt Pointer to a null-terminated format string containing format specifiers.
 * @param ... Variable arguments matching the format specifiers in `fmt`.
 *
 * @note This function assumes `itoa()` is implemented for converting integers.
 * @warning The function does not support floating-point numbers (`%f`).
 *
 * @example
 * @code
 * printf("Char: %c, String: %s, Int: %d\n", 'A', "Hello", 123); // Output: Char: A, String: Hello, Int: 123
 *
 * printf("Binary: %b, Octal: %o, Hex: %x\n", 10, 10, 10); // Output: Binary: 1010, Octal: 12, Hex: a
 *
 * printf("Percent: %%\n"); // Output: Percent: %
 * @endcode
 */
void printf(const char *fmt, ...);