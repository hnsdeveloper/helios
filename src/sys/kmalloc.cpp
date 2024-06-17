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
#include "mem/mmap.hpp"
#include "sys/mem.hpp"
#include "sys/panic.hpp"

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
                block->page_count * FrameKB::s_size - sizeof(MajorBlock) - sizeof(MinorBlock);
            if (hypotetical_free == block->free_space)
                return true;
        }
        return false;
    };

    bool fits_after(MajorBlock *block, size_t new_block_pages)
    {
        if (block == nullptr)
        {
            return true;
        }
        else
        {
            byte *end = as_byte_ptr(block) + block->page_count * FrameKB::s_size;
            byte *next_begin = as_byte_ptr(block->next);
            if (((end + new_block_pages * FrameKB::s_size) <= next_begin) || next_begin == nullptr)
                return true;
        }
        return false;
    }

    MajorBlock *find_list_candidate_predecessor(size_t pages)
    {
        if (begin_block)
        {
            for (auto block = begin_block; block; block = block->next)
                if (fits_after(block, pages))
                    return block;
        }
        return nullptr;
    }

    void major_setup(MajorBlock *block, size_t pages, MajorBlock *pred)
    {
        if (pred)
        {
            block->next = pred->next;
            pred->next = block;
        }

        block->page_count = pages;
        block->free_list = reinterpret_cast<MinorBlock *>(apply_offset(block, sizeof(MajorBlock)));
        block->free_space = block->page_count * FrameKB::s_size - sizeof(MajorBlock) - sizeof(MinorBlock);
        block->free_list->total_size = block->free_space;
        block->free_list->parent_block = block;
        block->free_list->p = nullptr;
    }

    MajorBlock *get_major_block(size_t pages)
    {
        const frame_info *f = framealloc(pages, 0);
        if (f == nullptr)
            return nullptr;
        MajorBlock *pred = find_list_candidate_predecessor(pages);
        byte *paddress = as_byte_ptr(f->frame_pointer);
        byte *vaddress;
        if (pred != nullptr)
            vaddress = as_byte_ptr(apply_offset(pred, pred->page_count * FrameKB::s_size));
        else
            vaddress = get_kernel_v_free_address();

        for (size_t i = 0; i < pages; ++i)
            kmmap(paddress + i * FrameKB::s_size, vaddress + i * FrameKB::s_size, get_kernel_pagetable(),
                  FrameLevel::FIRST_VPN, READ | WRITE | ACCESS | DIRTY, kmmap_frame_src);

        MajorBlock *block = reinterpret_cast<MajorBlock *>(vaddress);
        major_setup(block, pages, pred);
        return block;
    }

    void initialize_kmalloc()
    {
        begin_block = get_major_block(KMALLOC_INIT_BLOCKS);
        if (begin_block == nullptr)
            PANIC("Could not initialize kmalloc.");
    }

    void *malloc(size_t size)
    {
        // TODO: implemented
        return nullptr;
    }

} // namespace hls
