#include "alloc.h"
#include "lib.h"
#include "proc.h"
#include "riscv.h"
#include "sbi.h"  // TODO: Remove it
#include "trampoline.h"
#include "types.h"
#include "utils.h"

extern char __bss[], __bss_end[], __stack_top[];

struct process *proc_a;
struct process *proc_b;

void proc_a_entry(void) {
    printf("starting process A\n");
    while (1) {
        putchar('A');
        switch_context(&proc_a->sp, &proc_b->sp);
        yield();
    }
}

void proc_b_entry(void) {
    printf("starting process B\n");
    while (1) {
        putchar('B');
        switch_context(&proc_b->sp, &proc_a->sp);
        yield();
    }
}

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

// TODO: add comment later.
void init_boot(void) {
    INFO("Booting...");
    init_bss();
    init_trap_handler();
    init_idle_process();
    OK("Booted successfully.");
}

// TODO: add comment later.
void kernel_main(void) {
    init_boot();

    proc_a = create_process((uint32_t)proc_a_entry);
    proc_b = create_process((uint32_t)proc_b_entry);

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