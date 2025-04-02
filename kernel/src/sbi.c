#include "sbi.h"

#include "lib.h"
#include "sys.h"
#include "types.h"

/**
 * @brief Performs a Supervisor Binary Interface (SBI) call.
 *
 * This function executes an SBI call using the RISC-V `ecall` instruction,
 * allowing the operating system to communicate with the OpenSBI firmware.
 * The SBI call mechanism is used for various privileged operations,
 * such as console input/output, time management, and inter-processor
 * communication.
 *
 * The function takes eight arguments, which are placed in the appropriate
 * registers before executing the `ecall`. The SBI extension ID (`eid`) is
 * passed in register `a7`, and the function ID (`fid`) is passed in `a6`.
 * The remaining registers (`a0`-`a5`) hold additional parameters. Upon
 * return, `a0` contains either an error code (on failure) or a successful
 * return value, and `a1` may contain additional return data.
 *
 * @param[in] arg0  Value to be placed in register `a0` before the `ecall`.
 * @param[in] arg1  Value to be placed in register `a1` before the `ecall`.
 * @param[in] arg2  Value to be placed in register `a2` before the `ecall`.
 * @param[in] arg3  Value to be placed in register `a3` before the `ecall`.
 * @param[in] arg4  Value to be placed in register `a4` before the `ecall`.
 * @param[in] arg5  Value to be placed in register `a5` before the `ecall`.
 * @param[in] fid   Function ID, placed in register `a6`.
 * @param[in] eid   Extension ID, placed in register `a7`.
 *
 * @return A `sbiret` structure containing:
 *         - `error`: On success, it holds the returned value. On failure, it holds an error code.
 *         - `value`: Additional return data (if applicable).
 *
 * @note The caller must ensure that the correct arguments are passed
 *       according to the SBI specification for the requested function.
 *
 * @example
 * @code
 * struct sbiret ret = sbi_call('A', 0, 0, 0, 0, 0, 0, SYS_PUTCHAR);
 * if (ret.error >= 0) {
 *     printf("Character '%c' written successfully.\n", (char)ret.error);
 * } else {
 *     printf("SBI call failed with error code: %ld\n", ret.error);
 * }
 * @endcode
 */
struct sbiret sbi_call(int32_t arg0, int32_t arg1, int32_t arg2, int32_t arg3,
                       int32_t arg4, int32_t arg5, int32_t fid, int32_t eid) {
    register int32_t a0 __asm__("a0") = arg0;
    register int32_t a1 __asm__("a1") = arg1;
    register int32_t a2 __asm__("a2") = arg2;
    register int32_t a3 __asm__("a3") = arg3;
    register int32_t a4 __asm__("a4") = arg4;
    register int32_t a5 __asm__("a5") = arg5;
    register int32_t a6 __asm__("a6") = fid;
    register int32_t a7 __asm__("a7") = eid;

    __asm__ __volatile__("ecall" : "=r"(a0), "=r"(a1)
                         : "r"(a0), "r"(a1), "r"(a2), "r"(a3),
                           "r"(a4), "r"(a5), "r"(a6), "r"(a7)
                         : "memory");
    return (struct sbiret){.error = a0, .value = a1};
}

void putchar(char ch) {
    sbi_call(ch, 0, 0, 0, 0, 0, 0, SYS_PUTCHAR);
}

int32_t getchar(void) {
    while (true) {
        struct sbiret ret = sbi_call(0, 0, 0, 0, 0, 0, 0, SYS_GETCHAR);
        if (ret.error >= 0) return ret.error;
        // yield();
    }
}