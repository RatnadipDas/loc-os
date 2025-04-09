#pragma once
#include "types.h"

/**
 * @brief Performs a system call with up to three arguments.
 *
 * This function sets up the appropriate registers and executes the `ecall`
 * instruction to transition from user mode to supervisor mode, invoking
 * a system call identified by `sysno`.
 *
 * @param sysno The system call number (placed in register a3).
 * @param arg0  First argument to the syscall (passed in register a0).
 * @param arg1  Second argument to the syscall (passed in register a1).
 * @param arg2  Third argument to the syscall (passed in register a2).
 *
 * @return The result of the system call, returned in register a0.
 *
 * @note This function uses inline assembly to perform the syscall on a RISC-V
 * architecture. It clobbers memory to prevent the compiler from reordering
 * memory accesses around the syscall.
 */
int32_t syscall(int32_t sysno, int32_t arg0, int32_t arg1, int32_t arg2);

/**
 * @brief Writes a single character to the console output.
 *
 * This function performs a system call to output the character `ch`
 * to the console or serial output.
 *
 * @param ch The character to be printed.
 */
void putchar(char ch);

/**
 * @brief Reads a single character from the console input.
 *
 * This function performs a system call to read one character from
 * the console or serial input.
 *
 * @return The character read, or a negative value on error.
 */
int32_t getchar(void);

/**
 * @brief Reads data from a file into a buffer.
 *
 * This function invokes a system call to read up to `len` bytes from the file
 * specified by `filename` into the buffer `buf`.
 *
 * @param filename Name of the file to read from.
 * @param buf      Pointer to the buffer where data will be stored.
 * @param len      Maximum number of bytes to read.
 *
 * @return Number of bytes read on success, or -1 if the file was not found.
 */
int32_t readfile(const char *filename, char *buf, int32_t len);

/**
 * @brief Writes data from a buffer to a file.
 *
 * This function invokes a system call to write up to `len` bytes from the buffer
 * `buf` into the file specified by `filename`. If the file exists, it will be
 * overwritten with the new data.
 *
 * @param filename Name of the file to write to.
 * @param buf      Pointer to the buffer containing data to write.
 * @param len      Number of bytes to write.
 *
 * @return Number of bytes written on success, or -1 if the file was not found.
 */
int32_t writefile(const char *filename, const char *buf, int32_t len);

/**
 * @brief Shuts down the system.
 *
 * This function performs a system call to power off the machine.
 */
void shutdown(void);
