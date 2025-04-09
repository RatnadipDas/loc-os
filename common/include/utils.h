#pragma once
#include "colors.h"
#include "lib.h"

/**
 * @brief Logs a critical error message and halts execution.
 *
 * This macro prints a panic message in yellow (`[PANIC]`), including the file
 * name and line number, then enters an infinite loop to stop execution. The
 * loop uses the `wfi` (Wait For Interrupt) instruction to halt the CPU while
 * allowing external debugging or power-saving features.
 *
 * @param[in] fmt    The format string (similar to printf).
 * @param[in] ...    Additional arguments for formatting.
 *
 * @note This function does not return. It is intended for fatal errors where
 * the system must halt execution.
 *
 * @example
 * @code
 * PANIC("Kernel encountered an unrecoverable error!");
 * @endcode
 */
#define PANIC(fmt, ...)                                                        \
    do {                                                                       \
        printf("[" L_YELLOW "PANIC" NONE "] " L_BLACK "%s:%d: " NONE fmt "\n", \
               __FILE__, __LINE__, ##__VA_ARGS__);                             \
        for (;;) {                                                             \
            __asm__ __volatile__("wfi");                                       \
        }                                                                      \
    } while (false)

/**
 * @brief Logs a failure message.
 *
 * This macro prints a failure message in red (`[FAILED]`).
 *
 * @param[in] fmt    The format string (similar to printf).
 * @param[in] ...    Additional arguments for formatting.
 *
 * @example
 * @code
 * FAILED("Memory allocation failed!");
 * @endcode
 */
#define FAILED(fmt, ...) \
    printf("[" L_RED "FAILED" NONE "] " fmt "\n", ##__VA_ARGS__);

/**
 * @brief Logs a success message.
 *
 * This macro prints an OK message in green (`[  OK  ]`).
 *
 * @param[in] fmt    The format string (similar to printf).
 * @param[in] ...    Additional arguments for formatting.
 *
 * @example
 * @code
 * OK("System initialized successfully!");
 * @endcode
 */
#define OK(fmt, ...) \
    printf("[" L_GREEN "  OK  " NONE "] " fmt "\n", ##__VA_ARGS__);

/**
 * @brief Logs an informational message to the console.
 *
 * This macro prints a message with an light black (`[ INFO ]`) prefix in black text,
 * intended for general-purpose debugging or status updates during execution.
 * It helps track non-critical events in the system flow.
 *
 * @param[in] fmt   Format string (like `printf`), followed by optional arguments.
 *
 * @note This macro uses ANSI color codes for styling and `printf` for output.
 *
 * @example
 * @code
 * INFO("System boot complete.");
 * INFO("Driver loaded: %s", driver_name);
 * @endcode
 */
#define INFO(fmt, ...) \
    printf("[" L_BLACK " INFO " NONE "] " BLACK fmt "\n" NONE, ##__VA_ARGS__);
