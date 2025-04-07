#pragma once
#include "types.h"

// Page Table Entry (PTE) format for Sv32 (RISC-V):
//
// Bits:
// | 31 -------------------- 10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
// | PPN (Physical Page Number) | Resv. | D | A | G | U | X | W | R | V |
//
// Flags:
// D - Dirty:      Set if page has been written to
// A - Accessed:   Set if page has been read/written
// G - Global:     Shared across address spaces
// U - User:       Accessible from user mode
// X - Execute:    Executable page
// W - Write:      Writable page
// R - Read:       Readable page
// V - Valid:      Entry is valid (if not set, other bits are ignored)

#define SATP_SV32 (1u << 31)  // Enable Sv32 virtual memory mode (set mode field in satp register)

#define PAGE_V (1 << 0)  // Valid
#define PAGE_R (1 << 1)  // Readable
#define PAGE_W (1 << 2)  // Writable
#define PAGE_X (1 << 3)  // Executable
#define PAGE_U (1 << 4)  // User-accessible
#define PAGE_G (1 << 5)  // Global
#define PAGE_A (1 << 6)  // Accessed
#define PAGE_D (1 << 7)  // Dirty

/**
 * @brief Maps a virtual address to a physical address using a two-level page table.
 *
 * This function updates the page table entries to create a mapping from a given
 * virtual address (vaddr) to a physical address (paddr) with the specified flags.
 *
 * The mapping is done using a two-level page table:
 * - The first-level table (table1) has 1024 entries, indexed by the upper 10 bits
 *   of the virtual address (VPN1). Each entry points to a second-level page table.
 * - The second-level table also has 1024 entries, indexed by the next 10 bits of the
 *   virtual address (VPN0). Each entry maps a virtual page to a physical page.
 *
 * The page table entry (PTE) format for Sv32 is as follows:
 *
 * Bits: | 31 -------------------- 10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
 *       | PPN (Physical Page Number) | Resv. | D | A | G | U | X | W | R | V |
 *
 * The physical address is divided by PAGE_SIZE to extract the physical page number,
 * which is then shifted left by 10 bits (to occupy bits 31-10). The provided flags,
 * along with the VALID bit (PAGE_V), are OR'd into the lower bits of the PTE.
 *
 * @param table1 Pointer to the first-level page table.
 * @param vaddr Virtual address to be mapped.
 * @param paddr Physical address to map the virtual address to.
 * @param flags Additional flags for the mapping (e.g., PAGE_R, PAGE_W, PAGE_X, PAGE_U).
 */
void map_page(uint32_t *table1, uint32_t vaddr, paddr_t paddr, uint32_t flags);
