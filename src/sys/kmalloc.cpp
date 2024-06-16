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

#include "kmalloc.hpp"
#include "arch/riscv64gc/plat_def.hpp"
#include "mem/framemanager.hpp"
#include "sys/mem.hpp"

#define KMALLOC_INIT_BLOCKS 8

namespace hls
{

    struct MajorBlock;
    struct MinorBlock;

    struct MajorBlock
    {
        MajorBlock *next;
        size_t free_space;
        size_t page_count;
        MinorBlock *free_list;
    };

    struct MinorBlock
    {
        MajorBlock *parent_block;
        size_t total_size;
        void *p;
    };

    MajorBlock *begin_block = nullptr;
    MajorBlock *best_block = nullptr;

    bool is_major_empty(const MajorBlock *block)
    {
        if (block)
        {
            const size_t hypotetical_free =
                block->page_count * PAGE_FRAME_SIZE - sizeof(MajorBlock) - sizeof(MinorBlock);
            if (hypotetical_free == block->free_space)
                return true;
        }
        return false;
    };

    void initialize_kmalloc()
    {
        //const frame_info *f = framealloc(KMALLOC_INIT_BLOCKS, 0);
        //byte *begin = as_byte_ptr(f->frame_pointer);
        //byte* end = as_byte_ptr(apply_offset(begin, PAGE_FRAME_SIZE * KMALLOC_INIT_BLOCKS));
        //size_t total_size = abs_ptr_diff(end, begin);
        
        //begin_block = reinterpret_cast<MajorBlock *>(f->frame_pointer);
        //begin_block->page_count = KMALLOC_INIT_BLOCKS;
    }

    void *malloc(size_t size)
    {
        if (!begin_block)
            initialize_kmalloc();
        // TODO: implement

        return nullptr;
    }

} // namespace hls
