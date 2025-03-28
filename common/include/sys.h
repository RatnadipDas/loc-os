#pragma once

/**
 * @brief System call numbers for an operating system.
 *
 * These macros define system call numbers used to interact with the kernel.
 * Each system call performs a specific low-level operation.
 */
#define SYS_PUTCHAR 1    ///< Print a single character to the console.
#define SYS_GETCHAR 2    ///< Read a single character from input.
#define SYS_EXIT 3       ///< Terminate the process.
#define SYS_READFILE 4   ///< Read data from a file.
#define SYS_WRITEFILE 5  ///< Write data to a file.
#define SYS_SHUTDOWN 8   ///< Shutdown the system.