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

#include "include/arch/riscv/plat_def.h"
#include "misc/libfdt/libfdt.h"
#include "ulib/result.hpp"

namespace hls {

Result<PageLevel> walk_table(PageTable **table_ptr, const void *vaddress,
                             PageLevel current_vpn);

bool is_address_mapped(PageTable *table, void *vaddress);

Result<const void *> kmmap(PageTable *start_table, const void *vaddress,
                           PageLevel page_level, const void *physical_address,
                           bool writable = false, bool executable = false);

Result<const void *> get_physical_address(PageTable *start_table,
                                          const void *vaddress);
// TODO: IMPLEMENT
// void unkmmap(PageTable *start_table, void *vaddress, PageLevel page_level);

bool map_kernel(PageTable *table);

void *setup_kernel_memory_mapping(void *fdt);
} // namespace hls

#endif