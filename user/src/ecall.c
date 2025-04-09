#include "ecall.h"

#include "sys.h"
#include "types.h"

int32_t syscall(int32_t sysno, int32_t arg0, int32_t arg1, int32_t arg2) {
    register int32_t a0 __asm__("a0") = arg0;
    register int32_t a1 __asm__("a1") = arg1;
    register int32_t a2 __asm__("a2") = arg2;
    register int32_t a3 __asm__("a3") = sysno;

    __asm__ __volatile__("ecall"
                         : "=r"(a0)
                         : "r"(a0), "r"(a1), "r"(a2), "r"(a3)
                         : "memory");

    return a0;
}

void putchar(char ch) {
    syscall(SYS_PUTCHAR, ch, 0, 0);
}

int32_t getchar(void) {
    return syscall(SYS_GETCHAR, 0, 0, 0);
}

int32_t readfile(const char *filename, char *buf, int32_t len) {
    return syscall(SYS_READFILE, (int32_t)filename, (int32_t)buf, len);
}

int32_t writefile(const char *filename, const char *buf, int32_t len) {
    return syscall(SYS_WRITEFILE, (int32_t)filename, (int32_t)buf, len);
}

void shutdown(void) {
    syscall(SYS_SHUTDOWN, 0, 0, 0);
}
