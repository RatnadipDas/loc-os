#include "exit.h"

#include "ecall.h"
#include "lib.h"
#include "sys.h"

__attribute__((noreturn)) void exit(void) {
    syscall(SYS_EXIT, 0, 0, 0);
    while (true) {
        __asm__ __volatile__("wfi");
    };  // Just in case!
}
