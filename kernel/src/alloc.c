#include "alloc.h"

#include "lib.h"
#include "types.h"
#include "utils.h"

/**
 * @brief Symbols marking the boundaries of free RAM.
 *
 * These symbols are defined in the linker script and represent the start and end
 * of the free RAM region available for dynamic memory allocation (e.g., heap).
 *
 * - `__free_ram`: Start address of the free RAM.
 * - `__free_ram_end`: End address of the free RAM.
 */
extern char __free_ram[], __free_ram_end[];

paddr_t alloc_pages(uint32_t n) {
    static paddr_t next_paddr = (paddr_t)__free_ram;
    paddr_t paddr = next_paddr;
    next_paddr += n * PAGE_SIZE;

    if (next_paddr > (paddr_t)__free_ram_end)
        PANIC("out of memory");

    memset((void *)paddr, 0, n * PAGE_SIZE);
    return paddr;
}
