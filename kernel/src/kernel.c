#include "alloc.h"
#include "fs.h"
#include "lib.h"
#include "proc.h"
#include "riscv.h"
#include "trampoline.h"
#include "types.h"
#include "user.h"
#include "utils.h"
#include "virtio_disk.h"

/**
 * @brief Linker-defined symbols for memory section boundaries.
 *
 * These symbols are defined in the linker script and mark important memory
 * regions used during system startup and runtime:
 *
 * - `__bss`: Start address of the BSS segment (uninitialized global/static variables).
 * - `__bss_end`: End address of the BSS segment.
 * - `__stack_top`: Top of the stack (initial stack pointer value at boot).
 */
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

/**
 * @brief Initializes the trap handler.
 *
 * This function sets up the trap handler by configuring the `stvec`
 * register to point to the `trampoline` code, ensuring that traps
 * are handled correctly.
 *
 * It logs the initialization process using `INFO` before configuring
 * the `stvec` register and confirms completion with `OK`.
 *
 * @note This function should be called early in the system initialization
 * process to ensure proper exception and interrupt handling.
 *
 * @example
 * @code
 * init_trap_handler();
 * @endcode
 */
void init_trap_handler(void) {
    INFO("Initializing trap handler...");
    WRITE_CSR(stvec, (uint32_t)trampoline);
    OK("Initialized trap handler.");
}

/**
 * @brief Initializes the system during boot.
 *
 * This function performs early system initialization required to bring up
 * the kernel and prepare for user-space execution. The following steps
 * are executed in order:
 *
 * - Logs the boot message.
 * - Clears the BSS segment via `init_bss()`.
 * - Sets up the trap/interrupt handler with `init_trap_handler()`.
 * - Initializes the VirtIO block device using `init_virtio_blk()`.
 * - Creates the idle process with `init_idle_process()`.
 * - Creates the initial user process via `init_user()`.
 * - Initializes the filesystem with `init_fs()`.
 *
 * Finally, it logs a success message indicating that the system has booted.
 */
void init_boot(void) {
    INFO("Booting...");
    init_bss();
    init_trap_handler();
    init_virtio_blk();
    init_idle_process();
    init_user();
    init_fs();
    OK("Booted successfully.");
}

/**
 * @brief Entry point of the kernel after early boot.
 *
 * This function is called after the system has been bootstrapped. It performs
 * the final transition from kernel initialization to user-space execution.
 *
 * Steps performed:
 * - Calls `init_boot()` to initialize all subsystems.
 * - Logs a message indicating transition to the user shell.
 * - Calls `yield()` to switch context to the first user or idle process.
 * - Triggers a panic if control returns unexpectedly after yielding.
 * - Enters an infinite low-power wait loop (`wfi`) as a fallback.
 *
 * @note This function should never return under normal operation.
 */
void kernel_main(void) {
    init_boot();

    INFO("Switching to user shell...");
    yield();
    PANIC("switched to idle process");

    for (;;) {
        __asm__ __volatile__("wfi");
    };  // loop infinitely
}

__attribute__((section(".text.boot"))) __attribute__((naked))
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
void
boot(void) {
    __asm__ __volatile__(
        "mv sp, %[stack_top]\n"  // Set the stack pointer
        "j kernel_main\n"        // Jump to the kernel main function
        :
        : [stack_top] "r"(
            __stack_top)  // Pass the stack top address as %[stack_top]
    );
}
