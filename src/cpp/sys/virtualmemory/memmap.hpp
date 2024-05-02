/*---------------------------------------------------------------------------------
MIT License

Copyright (c) 2024 Helio Nunes Santos

        Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
        copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
        copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
        AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

---------------------------------------------------------------------------------*/

#ifndef _MEMMAP_HPP_
#define _MEMMAP_HPP_

#include "include/types.h"
#include "include/arch/riscv/plat_def.h"
#include "misc/libfdt/libfdt.h"
#include "ulib/result.hpp"
#include "ulib/map.hpp"

namespace hls {

/**
 * @brief Walks a page table till it can't walk it anymore (either because it is not mapped or because it is currently mapped to a bigger level)
 * 
 * @param table_ptr The page table
 * @param vaddress The virtual address we are trying to reach
 * @param current_vpn Page level corresponding to table_ptr
 * @return Result<PageLevel> Returns an error or the next page level, while modifying table_ptr to point to the next table. If it reachs a leaf, subsequent calls will not have any effect.
 */
Result<PageLevel> walk_table(PageTable **table_ptr, const void *vaddress,
                             PageLevel current_vpn);


/**
 * @brief Checks if a given virtual address is mapped
 * 
 * @param table The page table where to check if the address is mapped
 * @param vaddress 
 * @return true 
 * @return false 
 */
bool is_address_mapped(PageTable *table, void *vaddress);


/**
 * @brief Maps a virtual address to a virtual address.
 * 
 * @param paddress Physical address to which a virtual address will be pointing to.
 * @param vaddress Virtual address to be used by the kernel or user land software.
 * @param table The root page table.
 * @param page_level The level of pagination to which the virtual address will be pointing to. Note that paddress and vaddress must be aligned to this.
 * @param writable Enables writing to that region.
 * @param executable Enables executing from that region.
 * @return Result<const void *> 
 */
Result<const void *> kmmap(const void* paddress, const void* vaddress, PageTable *table, PageLevel page_level,
                           bool writable = false, bool executable = false);


void kmunmap(const void* vaddress, PageTable* table);

/**
 * @brief Get the physical address corresponding to a virtual address
 * 
 * @param start_table The table which we wish to walk to get the physical address
 * @param vaddress The virtual address which we are trying to find 
 * @return Result<const void *> Returns the physical address or error.
 */
Result<void *> get_physical_address(PageTable *start_table,
                                          const void *vaddress);

// TODO: IMPLEMENT
// void unkmmap(PageTable *start_table, void *vaddress, PageLevel page_level);

/**
 * @brief Maps the kernel to a page table.
 * 
 * @param table The table which the kernel is being mapped to.
 * @return true If the kernel was mapped succesfully to the page table.
 * @return false If the kernel wasn't mapped successfully to the page table.
 */
bool map_kernel(PageTable *table);

/**
 * @brief Creates the kernel page table
 * 
 * @return PageTable* Pointer to the kernel page table
 */
PageTable *setup_kernel_memory_mapping();

} // namespace hls

#endif