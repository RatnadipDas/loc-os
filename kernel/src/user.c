#include "user.h"

#include "proc.h"
#include "types.h"
#include "utils.h"

/**
 * @brief Start address of the embedded user program binary.
 *
 * This symbol is defined by the linker and marks the beginning of the
 * user program binary, which has been embedded into the kernel binary
 * using objcopy.
 *
 * @note This is not a normal variable, but a symbol pointing to the start
 *       of the raw binary data.
 */
extern char _binary_build_user_user_bin_start[];

/**
 * @brief Size of the embedded user program binary.
 *
 * This symbol is defined by the linker and represents the total size
 * (in bytes) of the user program binary embedded in the kernel.
 *
 * @note This is not a variable but a symbol whose address holds the binary size.
 */
extern char _binary_build_user_user_bin_size[];

/**
 * @brief Transfers control from supervisor mode to user mode.
 *
 * This function prepares the necessary Control and Status Registers (CSRs)
 * and uses the `sret` instruction to switch from supervisor mode to user mode.
 *
 * It sets:
 * - `sepc`: The exception program counter, to point to the beginning of the user program (`USER_BASE`).
 * - `sstatus`: Sets the `SPIE` bit to enable interrupts in user mode after the switch,
 *              and the `SUM` bit to allow supervisor-mode code to access user memory.
 *
 * @note This function does not return. After `sret`, execution continues in user mode
 *       at the address specified in `sepc`.
 */
void user_entry(void) {
    __asm__ __volatile__(
        "csrw sepc, %[sepc]        \n"
        "csrw sstatus, %[sstatus]  \n"
        "sret                      \n"
        :
        : [sepc] "r"(USER_BASE),                     // pc to USER_BASE
          [sstatus] "r"(SSTATUS_SPIE | SSTATUS_SUM)  // switch to user mode, permit Supervisor to access User memory
    );
}

void init_user(void) {
    INFO("Initializing user process...");
    create_process(_binary_build_user_user_bin_start, (size_t)_binary_build_user_user_bin_size, USER_BASE, (const vaddr_t)user_entry);
    OK("Initialized user process.");
}
