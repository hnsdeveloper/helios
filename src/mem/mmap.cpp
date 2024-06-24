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

#include "mem/mmap.hpp"
#include "mem/framemanager.hpp"
#include "sys/mem.hpp"
#include "sys/opensbi.hpp"
#include "sys/print.hpp"

namespace hls
{

    static PageTable *s_scratch_page_table;
    static PageTable *s_kernel_page_table;
    static byte *s_kernel_free_addr;

    void set_kernel_v_free_address(byte *vaddress)
    {
        s_kernel_free_addr = vaddress;
    }

    byte *get_kernel_v_free_address()
    {
        return s_kernel_free_addr;
    }

    void set_scratch_pagetable(PageTable *vpaddress)
    {
        s_scratch_page_table = vpaddress;
    }

    PageTable *get_scratch_pagetable()
    {
        return s_scratch_page_table;
    }

    void set_kernel_pagetable(PageTable *paddress)
    {
        s_kernel_page_table = paddress;
    }

    PageTable *get_kernel_pagetable()
    {
        return s_kernel_page_table;
    }

    PageTable *translated_page_vaddress(PageTable *paddress)
    {
        auto scratch = get_scratch_pagetable();
        auto &entry = scratch->get_entry(PageTable::entries_on_table - get_cpu_id() - 2);
        entry.point_to_frame(paddress);
        entry.set_flags(READ | WRITE | ACCESS | DIRTY);
        flush_tlb();
        return (PageTable *)(nullptr) - get_cpu_id() - 2;
    }

    size_t get_page_entry_index(const void *vaddress, FrameOrder v)
    {
        size_t vpn_idx = static_cast<size_t>(v);
        uintptr_t idx = to_uintptr_t(vaddress) >> 12;
        return (idx >> (vpn_idx * 9)) & 0x1FF;
    }

    uintptr_t get_vaddress_offset(const void *vaddress)
    {
        uintptr_t p = to_uintptr_t(vaddress);
        return p & 0xFFF;
    }

    void walk_table(const void *vaddress, PageTable **table, FrameOrder *lvl)
    {
        if (table == nullptr || lvl == nullptr || *lvl == FrameOrder::LOWEST_ORDER)
            return;

        size_t idx = get_page_entry_index(vaddress, *lvl);
        auto &entry = translated_page_vaddress(*table)->get_entry(idx);

        if (entry.is_valid() && !entry.is_leaf())
        {
            *table = entry.as_table_pointer();
            *lvl = next_vpn(*lvl);
        }
    }

    void *v_to_p(const void *vaddress, PageTable *table = nullptr)
    {
        if (table != nullptr)
        {

            constexpr size_t p_lvl_count = (size_t)(FrameOrder::HIGHEST_ORDER);
            FrameOrder lvl = FrameOrder::HIGHEST_ORDER;
            for (size_t i = 0; i < p_lvl_count; ++i)
            {
                // Will walk the table as much as it can.
                // If it is a superpage, then it won't walk more, and then we only apply the offset to the address
                walk_table(vaddress, &table, &lvl);
            }

            auto &entry = table->get_entry(get_page_entry_index(vaddress, lvl));

            if (entry.is_valid() && entry.is_leaf())
            {
                return apply_offset(entry.as_pointer(), get_vaddress_offset(vaddress));
            }
        }
        return nullptr;
    }

    void direct_frame_map(const void *paddress, const void *vaddress, PageTable *table, const FrameOrder lvl,
                          uint64_t flags)
    {
        PageTable *vt = translated_page_vaddress(table);
        size_t lvl_entry_idx = get_page_entry_index(vaddress, lvl);
        auto &entry = vt->get_entry(lvl_entry_idx);
        entry.point_to_frame(paddress);
        entry.set_flags(flags);
    }

    bool kmmap(const void *paddress, const void *vaddress, PageTable *table, const FrameOrder p_lvl, uint64_t flags,
               frame_fn f_src)
    {

        if (table == nullptr)
            return false;

        if (vaddress == get_scratch_pagetable())
            return false;

        FrameOrder c_lvl = FrameOrder::HIGHEST_ORDER;
        FrameOrder expected = next_vpn(c_lvl);
        PageTable *p_table = table;

        while (c_lvl != p_lvl)
        {
            walk_table(vaddress, &p_table, &c_lvl);
            if (c_lvl == expected)
            {
                expected = next_vpn(expected);
                continue;
            }

            PageTable *vt = translated_page_vaddress(p_table);
            size_t idx = get_page_entry_index(vaddress, c_lvl);
            auto *entry = &(vt->get_entry(idx));

            // Address already mapped
            if (entry->is_leaf())
                return false;

            if (!(entry->is_valid()))
            {
                // If entry is not valid, either it has been swapped or it never been mapped
                // TODO: add code to check for swapped pages
                void *frame = f_src();
                if (frame == nullptr)
                {
                    // TODO: add proper error handling
                    return false;
                }
                entry->point_to_table(reinterpret_cast<PageTable *>(frame));
                auto vt = translated_page_vaddress(reinterpret_cast<PageTable *>(frame));
                memset(vt, 0, PAGE_FRAME_SIZE);
                // Recovering t, given that we have mapped other page table
                vt = translated_page_vaddress(p_table);
            }

            c_lvl = next_vpn(c_lvl);
            expected = next_vpn(expected);
            p_table = entry->as_table_pointer();
        }

        direct_frame_map(paddress, vaddress, p_table, c_lvl, flags);
        return true;
    }

    void kmunmap(const void *vaddress, PageTable *ptable, frame_rls_fn rls_fn)
    {
        if (vaddress == nullptr || ptable == nullptr)
            return;

        // We assume we are using the highest possible page level.
        FrameOrder current_level = FrameOrder::HIGHEST_ORDER;

        // Stores all pages used to reach the address
        PageTable *table_path[(size_t)(FrameOrder::HIGHEST_ORDER) + 1];

        size_t i = 0;
        while (current_level != FrameOrder::FIRST_ORDER)
        {
            table_path[i++] = ptable;
            // If the current page contains a leaf node, the page will not be walked.
            // Thus later it will break out of the loop
            walk_table(vaddress, &ptable, &current_level);
            PageTable *vtable = translated_page_vaddress(ptable);

            size_t idx = get_page_entry_index(vaddress, current_level);
            TableEntry &entry = vtable->get_entry(idx);

            if (!entry.is_valid())
                return;

            if (entry.is_leaf())
                break;
        }

        PageTable *vtable = translated_page_vaddress(ptable);
        auto &entry = vtable->get_entry(get_page_entry_index(vaddress, current_level));
        entry.erase();

        bool freed = false;

        while (true)
        {
            PageTable *ph_table = table_path[--i];
            PageTable *vh_table = translated_page_vaddress(ph_table);

            if (freed)
            {
                auto &entry = vh_table->get_entry(get_page_entry_index(vaddress, current_level));
                entry.erase();
            }

            if (vh_table->is_empty())
            {
                rls_fn(ph_table);
                freed = true;
            }

            current_level = next_vpn(current_level);

            if (i == 0)
                break;
        }
    }

    size_t kmmap(const void *paddress, const void *vaddress, PageTable *table, const FrameOrder p_lvl, uint64_t flags,
                 FrameKB **f_src, size_t f_count)
    {

        if (table == nullptr)
            return false;

        if (vaddress == get_scratch_pagetable())
            return false;

        FrameOrder c_lvl = FrameOrder::HIGHEST_ORDER;
        FrameOrder expected = next_vpn(c_lvl);
        PageTable *p_table = table;
        size_t used_frames = 0;
        while (c_lvl != p_lvl)
        {
            walk_table(vaddress, &p_table, &c_lvl);
            if (c_lvl == expected)
            {
                expected = next_vpn(expected);
                continue;
            }

            PageTable *vt = translated_page_vaddress(p_table);
            size_t idx = get_page_entry_index(vaddress, c_lvl);
            auto *entry = &(vt->get_entry(idx));

            // Address already mapped
            if (entry->is_leaf())
                return false;

            if (!(entry->is_valid()))
            {
                // If entry is not valid, either it has been swapped or it never been mapped
                // TODO: add code to check for swapped pages
                if (f_src == nullptr || used_frames == f_count)
                {
                    // TODO: add proper error handling
                    return used_frames;
                }
                void *frame = f_src[used_frames];
                f_src[used_frames++] = nullptr;

                entry->point_to_table(reinterpret_cast<PageTable *>(frame));
                auto vt = translated_page_vaddress(reinterpret_cast<PageTable *>(frame));
                memset(vt, 0, PAGE_FRAME_SIZE);
                // Recovering t, given that we have mapped other page table
                vt = translated_page_vaddress(p_table);
            }

            c_lvl = next_vpn(c_lvl);
            expected = next_vpn(expected);
            p_table = entry->as_table_pointer();
        }
        direct_frame_map(paddress, vaddress, p_table, c_lvl, flags);
        return used_frames;
    }

    bool kmunmap(const void *vaddress, PageTable *ptable, FrameKB **f_dst, const size_t &limit)
    {
        if (vaddress == nullptr || ptable == nullptr)
            return false;

        // We assume we are using the highest possible page level.
        FrameOrder current_level = FrameOrder::HIGHEST_ORDER;

        // Stores all pages used to reach the address
        PageTable *table_path[(size_t)(FrameOrder::HIGHEST_ORDER) + 1];

        size_t i = 0;
        while (current_level != FrameOrder::FIRST_ORDER)
        {
            table_path[i++] = ptable;
            // If the current page contains a leaf node, the page will not be walked.
            // Thus later it will break out of the loop
            walk_table(vaddress, &ptable, &current_level);
            PageTable *vtable = translated_page_vaddress(ptable);

            size_t idx = get_page_entry_index(vaddress, current_level);
            TableEntry &entry = vtable->get_entry(idx);

            if (!entry.is_valid())
            {
                // HANDLE SWAPPED PAGES?
                return false;
            }

            if (entry.is_leaf())
                break;
        }

        PageTable *vtable = translated_page_vaddress(ptable);
        auto &entry = vtable->get_entry(get_page_entry_index(vaddress, current_level));
        entry.erase();

        bool freed = false;
        size_t released = 0;
        while (true)
        {
            PageTable *ph_table = table_path[--i];
            PageTable *vh_table = translated_page_vaddress(ph_table);

            if (freed)
            {
                auto &entry = vh_table->get_entry(get_page_entry_index(vaddress, current_level));
                entry.erase();
            }

            if (vh_table->is_empty())
            {
                if (released < limit)
                {
                    f_dst[released] = ph_table;
                    ++released;
                    freed = true;
                }
                else
                {
                    return false;
                }
            }

            current_level = next_vpn(current_level);

            if (i == 0)
                break;
        }

        return true;
    }

    void setup_kernel_memory_map(bootinfo *b_info)
    {
        set_kernel_pagetable(b_info->p_kernel_table);
        set_scratch_pagetable(b_info->v_scratch);
        set_kernel_v_free_address(b_info->v_highkernel_end);
    }

    void unmap_low_kernel(byte *begin, byte *end)
    {
        PageTable *kernel_table = get_kernel_pagetable();
        for (auto it = begin; it < end; it += PAGE_FRAME_SIZE)
        {
            kmunmap(it, kernel_table, initffree);
        }
    }

} // namespace hls
