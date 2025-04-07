#include "vm.h"

#include "alloc.h"
#include "arg.h"
#include "lib.h"
#include "types.h"
#include "utils.h"

void map_page(uint32_t *table1, uint32_t vaddr, paddr_t paddr, uint32_t flags) {
    if (!is_aligned(vaddr, PAGE_SIZE))
        PANIC("Unaligned paddr: 0x%x", vaddr);

    if (!is_aligned(paddr, PAGE_SIZE))
        PANIC("Unaligned vaddr: 0x%x", paddr);

    // Get upper 10 bits of virtual address and set every bit other than lower 10 bits to zero
    uint32_t vpn1 = (vaddr >> 22) & 0x3ff;  // 0x3ff == 1024

    // Get upper 20 bits of virtual address and set every bit other than lower 10 bits to zero
    uint32_t vpn0 = (vaddr >> 12) & 0x3ff;  // 0x3ff == 1024

    if ((table1[vpn1] & PAGE_V) == 0) {  // If PTE(page table entry) is not mapped to any physical address of 2nd level address (VALID bit is 0)
        // Create the non-existent 2nd level page entry
        uint32_t pt_paddr = alloc_pages(1);

        // Page Table Entry (PTE) format for Sv32 (RISC-V):
        //
        // Bits:
        // | 31 -------------------- 10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
        // | PPN (Physical Page Number) | Resv. | D | A | G | U | X | W | R | V |
        //
        // PNN - Physical Page Number: upper 20 bits of 2nd level Page Table address
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

        // Build the first-level PTE:
        // - pt_paddr / PAGE_SIZE: remove lower 12 zero bits from physical address of 2nd level address
        //                         now pt_paddr is in range 19 to 0 bits
        // - Shift it by 10, now it occupies 29 to 10 (within 31 to 10 range)
        // - Set VALID bit to 1 (PAGE_V) and store it in PTE
        //
        // Each PTE is of size 4 Bytes
        table1[vpn1] = ((pt_paddr / PAGE_SIZE) << 10) | PAGE_V;
    }

    // Get 2nd level Page Table address form 1st level PTE
    uint32_t *table0 = (uint32_t *)((table1[vpn1] >> 10) * PAGE_SIZE);

    // Page Table Entry (PTE) format for Sv32 (RISC-V):
    //
    // Bits:
    // | 31 -------------------- 10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
    // | PPN (Physical Page Number) | Resv. | D | A | G | U | X | W | R | V |
    //
    // PNN - Physical Page Number: upper 20 bits of Physical page address
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

    // Build the second-level PTE for the virtual page:
    // - paddr / PAGE_SIZE: remove lower 12 zero bits from physical address of 2nd level address
    //                      now paddr is in range 19 to 0 bits
    // - Shift it by 10, now it occupies 29 to 10 (within 31 to 10 range)
    // - Set VALID bit to 1 (PAGE_V), set other read, write etc bits provided and store it in PTE
    //
    // Each PTE is of size 4 Bytes
    table0[vpn0] = ((paddr / PAGE_SIZE) << 10) | flags | PAGE_V;
}
