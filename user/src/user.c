#include "exit.h"
#include "shell.h"

/**
 * @brief Symbol marking the top of the stack.
 *
 * This symbol is defined in the linker script and represents the initial
 * stack pointer value used at system startup.
 *
 * @note This is used during low-level initialization to set up the stack
 * pointer before entering `main` or other higher-level code.
 */
extern char __stack_top[];

__attribute__((section(".text.start")))
__attribute__((naked))
/**
 * @brief Kernel entry point set at boot time.
 *
 * This is the very first function executed after the system boots. It performs
 * the essential setup to transfer control to the `main` function.
 *
 * Behavior:
 * - Sets the stack pointer (`sp`) to the address of `__stack_top`.
 * - Calls the `main()` function.
 * - If `main()` returns, it calls `exit()` to terminate the process cleanly.
 *
 * Attributes:
 * - `section(".text.start")`: Places this function in a special section, typically
 *   mapped to the entry point in the linker script or bootloader.
 * - `naked`: Prevents the compiler from generating prologue/epilogue code, allowing
 *   full control over register and stack setup via inline assembly.
 *
 * @note This function should be linked as the entry point (`start`) and should never return.
 */
void
start(void) {
    __asm__ __volatile__(
        "mv sp, %[stack_top] \n"
        "call main           \n"
        "call exit           \n" ::[stack_top] "r"(__stack_top));
}
