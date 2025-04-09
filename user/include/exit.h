#pragma once

__attribute__((noreturn))
/**
 * @brief Terminates the current process.
 *
 * This function performs a system call to mark the current process as exited.
 * It does not return to the caller. In case the system call fails or returns
 * unexpectedly, it enters an infinite low-power wait loop.
 *
 * @note This function is marked with `noreturn` to indicate that it does not return.
 */
void
exit(void);
