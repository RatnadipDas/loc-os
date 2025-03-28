#include "kernel.h"

#include "lib.h"
#include "types.h"

extern char __bss[], __bss_end[], __stack_top[];

void kernel_main(void) {
  memset(__bss, 0,
         (size_t)__bss_end - (size_t)__bss);  // set the bss memory area to zero

  for (;;);  // loop infinitely
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