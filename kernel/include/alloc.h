#pragma once
#include "types.h"

/**
 * @brief Allocates a contiguous block of physical memory pages.
 *
 * @param n The number of pages to allocate.
 * @return The starting physical address (`paddr_t`) of the allocated memory block.
 *
 * @note The function maintains a static pointer (`next_paddr`) to track the
 * next available physical address for allocation.
 * @note If there is not enough free memory available, the function triggers
 * a system panic (`PANIC("out of memory")`).
 * @note The allocated memory is zero-initialized using `memset`.
 *
 * @warning This function does not implement deallocation. Once allocated,
 * the memory cannot be freed.
 *
 * @example
 * @code
 * paddr_t page1 = alloc_pages(1); // Allocate 1 page
 * paddr_t page2 = alloc_pages(4); // Allocate 4 pages
 * @endcode
 */
paddr_t alloc_pages(uint32_t n);
