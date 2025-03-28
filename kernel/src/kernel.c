#include "lib.h"
#include "types.h"

extern char __bss[], __bss_end[], __stack_top[];

/**
 * @brief Initializes the .bss section by setting it to zero.
 *
 * This function clears the `.bss` section by filling it with zeros.
 * The `.bss` section typically contains uninitialized global and static
 * variables.
 *
 * It logs the initialization process using `INFO` before clearing the memory
 * and confirms completion with `OK`.
 *
 * @note This function should be called early in the system initialization
 * process.
 *
 * @example
 * @code
 * init_bss();
 * @endcode
 */
void init_bss(void) {
    INFO("Initializing .bss area...");
    memset(__bss, 0, (size_t)__bss_end - (size_t)__bss);
    OK("Initialized .bss area.");
}

void init_boot(void) {
    INFO("Booting...");
    init_bss();
    OK("Booted successfully.");
}

void kernel_main(void) {
    init_boot();

    PANIC("panics here!");
    FAILED("unreachable here!");

    for (;;) {
        __asm__ __volatile__("wfi");
    };  // loop infinitely
}

/**
 * @brief Boot function executed at system startup.
 *
 * This function is placed in the `.text.boot` section and is marked `naked` to
 * prevent compiler-generated prologue/epilogue code. It sets up the stack
 * pointer and jumps to `kernel_main()`.
 *
 * @note This function must be the first executed function in the system.
 * @note It is written in inline assembly for precise control over execution
 * flow.
 *
 * @details
 * - The `mv sp, %[stack_top]` instruction initializes the stack pointer.
 * - The `j kernel_main` instruction transfers control to the kernel's main
 * function.
 * - The `__stack_top` symbol is provided by the linker script and represents
 * the top of the stack.
 */
__attribute__((section(".text.boot"))) __attribute__((naked)) void boot(void) {
    __asm__ __volatile__(
        "mv sp, %[stack_top]\n"  // Set the stack pointer
        "j kernel_main\n"        // Jump to the kernel main function
        :
        : [stack_top] "r"(
            __stack_top)  // Pass the stack top address as %[stack_top]
    );
}