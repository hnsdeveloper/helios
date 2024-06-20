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

#include "sys/kmalloc.hpp"
#include "arch/riscv64gc/plat_def.hpp"
#include "mem/framemanager.hpp"
#include "mem/mmap.hpp"
#include "sys/mem.hpp"
#include "sys/panic.hpp"
#include "sys/print.hpp"

#define KMALLOC_INIT_BLOCKS 8

constexpr size_t MINOR_BLOCK_MAGIC = 0x50173400deadbeef;

namespace hls
{
    struct MajorBlock;
    struct MinorBlock;

    struct MajorBlock
    {
        MajorBlock *next;
        MinorBlock *free_list;
        size_t free_space;
        size_t page_count;
    };

    struct MinorBlock
    {
        MajorBlock *parent_block;
        MinorBlock *next;
        size_t total_size;
        size_t magic;
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
        block->next = nullptr;
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
        block->free_list->next = nullptr;
        block->free_list->magic = MINOR_BLOCK_MAGIC;
    }

    void minor_setup(MinorBlock *block, size_t size, MajorBlock *parent, MinorBlock *next)
    {
        block->parent_block = parent;
        block->total_size = size;
        block->next = next;
        block->magic = MINOR_BLOCK_MAGIC;
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

    MinorBlock *find_suitable_minor_block(MajorBlock *block, size_t size)
    {
        MinorBlock *minor = block->free_list;
        MinorBlock *suitable = nullptr;
        while (minor)
        {
            byte *begin_mem = as_byte_ptr(minor) + sizeof(MinorBlock);
            byte *end_mem = as_byte_ptr(minor) + minor->total_size;
            byte *mem = as_byte_ptr(align_forward(begin_mem, alignof(max_align_t)));
            if ((mem + size) <= end_mem)
            {
                if (suitable == nullptr)
                    suitable = minor;
                else
                    suitable = suitable->total_size < minor->total_size ? suitable : minor;
            }
            minor = minor->next;
        }
        return suitable;
    }

    void split_minor(MinorBlock *prev, MinorBlock *block, size_t size)
    {
        byte *mem = as_byte_ptr(align_forward(apply_offset(block, sizeof(MinorBlock)), alignof(max_align_t)));
        byte *mem_end = as_byte_ptr(align_forward(apply_offset(block, size), alignof(MinorBlock)));
        size_t used_size = mem_end - mem;
        block->parent_block->free_space = block->parent_block->free_space - used_size;
        if ((used_size < size) && ((block->total_size - used_size) > sizeof(MinorBlock *)))
        {
            MinorBlock *sub = reinterpret_cast<MinorBlock *>(mem_end);
            minor_setup(sub, block->total_size - used_size, block->parent_block, block->next);
            block->total_size = used_size;
            if (prev)
                prev->next = sub;
            else
                block->parent_block->free_list = sub;
            block->parent_block->free_space = block->parent_block->free_space - sizeof(MinorBlock);
        }
    }

    void *malloc(size_t size)
    {
        MajorBlock *used_major;
        MinorBlock *used_minor;

        // First try to find on the best block.
        if (best_block)
        {
            MinorBlock *temp = find_suitable_minor_block(best_block, size);
            if (temp != nullptr)
            {
                used_major = best_block;
                used_minor = temp;
            }
            else
            {
                kdebug("No suitable minor block found at best_block.");
                best_block = nullptr;
            }
        }

        // Okay, we didn't find anything on the best block, time to search all blocks
        if (!best_block)
        {
            for (MajorBlock *it = begin_block; begin_block; it = it->next)
            {
                MinorBlock *temp = find_suitable_minor_block(it, size);
                if (temp != nullptr)
                {
                    used_major = it;
                    used_minor = temp;
                    break;
                }
            }
            if (!used_major && !used_minor)
            {
                kdebug("No suitable minor block found on any of current allocated major blocks.");
            }
        }

        // Okay, nothing found on any of the blocks, time to allocate blocks!
        if (!used_major && !used_minor)
        {
            size_t needed_frames =
                size / FrameKB::s_size + 1; // Extra block for the case where size == KMALLOC_INIT_BLOCKS
            if (needed_frames < KMALLOC_INIT_BLOCKS)
                needed_frames = KMALLOC_INIT_BLOCKS;
            best_block = get_major_block(needed_frames);
            if (!best_block)
                PANIC("Out of memory."); // We could return nullptr, but given that it is kernel code, if an allocation
            // fails, it is a big issue
            used_major = best_block;
            used_minor = best_block->free_list;
        }

        MinorBlock *prev = [](MinorBlock *min) -> MinorBlock * {
            MinorBlock *p = min->parent_block->free_list;
            if (p == min)
                return nullptr;
            while (p->next != min)
                p = p->next;
            return p;
        }(used_minor);

        split_minor(prev, used_minor, size);
        return align_forward(apply_offset(used_minor, sizeof(used_minor)), alignof(max_align_t));
    }

    void free(void *ptr)
    {
    }

} // namespace hls
