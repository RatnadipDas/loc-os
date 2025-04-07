#pragma once
#include "types.h"

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