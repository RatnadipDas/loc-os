#pragma once

/**
 * @brief Boolean type definition.
 *
 * Defines `bool` as an integer type, typically used for representing boolean
 * values.
 *
 * @note Conventionally, `true` is represented as `1` and `false` as `0`.
 */
typedef int bool;

/**
 * @brief Fixed-width integer type definitions.
 *
 * These typedefs define integer types with specific bit widths, ensuring
 * portability across different platforms.
 */
typedef char int8_t;            ///< 8-bit signed integer.
typedef unsigned char uint8_t;  ///< 8-bit unsigned integer.

typedef short int16_t;            ///< 16-bit signed integer.
typedef unsigned short uint16_t;  ///< 16-bit unsigned integer.

typedef int int32_t;            ///< 32-bit signed integer.
typedef unsigned int uint32_t;  ///< 32-bit unsigned integer.

typedef long long int64_t;            ///< 64-bit signed integer.
typedef unsigned long long uint64_t;  ///< 64-bit unsigned integer.

/**
 * @brief Type definitions for memory-related values.
 *
 * These types are typically used in memory management, addressing, and buffer
 * size representations.
 */
typedef uint32_t
    size_t;  ///< Type used for representing sizes of objects in memory.
typedef uint32_t paddr_t;  ///< Type used for representing physical addresses.
typedef uint32_t vaddr_t;  ///< Type used for representing virtual addresses.