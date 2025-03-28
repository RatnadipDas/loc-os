#include "sbi.h"

#include "sys.h"
#include "types.h"

/**
 * @brief Performs an SBI (Supervisor Binary Interface) call.
 *
 * This function executes an `ecall` instruction to invoke an SBI function
 * with the provided arguments and returns the result.
 *
 * @param[in] arg0 First argument for the SBI call (a0 register).
 * @param[in] arg1 Second argument for the SBI call (a1 register).
 * @param[in] arg2 Third argument for the SBI call (a2 register).
 * @param[in] arg3 Fourth argument for the SBI call (a3 register).
 * @param[in] arg4 Fifth argument for the SBI call (a4 register).
 * @param[in] arg5 Sixth argument for the SBI call (a5 register).
 * @param[in] fid SBI function ID (a6 register).
 * @param[in] eid SBI extension ID (a7 register).
 *
 * @return A `struct sbiret` containing:
 *   - `error`: A status code (0 on success, negative on failure).
 *   - `value`: The return value from the SBI call.
 *
 * @note This function is used in RISC-V operating systems or firmware to
 * interact with lower privilege levels (e.g., from supervisor mode to machine
 * mode).
 *
 * @example
 * @code
 * struct sbiret result = sbi_call(0, 0, 0, 0, 0, 0, 0, SYS_SHUTDOWN);
 * if (result.error) {
 *     printf("SBI call failed with error: %ld\n", result.error);
 * } else {
 *     printf("SBI call succeeded, value: %ld\n", result.value);
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