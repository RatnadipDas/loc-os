#include "user.h"

#include "proc.h"
#include "types.h"
#include "utils.h"

extern char _binary_build_user_user_bin_start[], _binary_build_user_user_bin_size[];

// TODO: add comment later
void user_entry(void) {
    __asm__ __volatile__(
        "csrw sepc, %[sepc]        \n"
        "csrw sstatus, %[sstatus]  \n"
        "sret                      \n"
        :
        : [sepc] "r"(USER_BASE),
          [sstatus] "r"(SSTATUS_SPIE));
}

void init_user(void) {
    INFO("Initializing user process...");
    create_process(_binary_build_user_user_bin_start, (size_t)_binary_build_user_user_bin_size, USER_BASE, (const vaddr_t)user_entry);
    OK("Initialized user process.");
}