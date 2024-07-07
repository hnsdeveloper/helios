/*---------------------------------------------------------------------------------
MIT License

Copyright (c) 2024 Helio Nunes Santos

        Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"), to
deal in the Software without restriction , including without limitation the
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
#include "ulib/pair.hpp"

namespace hls
{

    size_t get_page_entry_index(const void *vaddress, FrameOrder v)
    {
        size_t vpn_idx = static_cast<size_t>(v);
        uintptr_t idx = to_uintptr_t(vaddress) >> 12;
        return (idx >> (vpn_idx * 9)) & 0x1FF;
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

    Pair<FrameOrder, PageTable *> table_walk(const void *vaddress, PageTable *table, FrameOrder order)
    {
        if (table != nullptr && order != FrameOrder::LOWEST_ORDER)
        {
            size_t idx = get_page_entry_index(vaddress, order);
            auto v_table = translated_page_vaddress(table);
            auto &entry = v_table->get_entry(idx);
            if (entry.is_valid() && !entry.is_leaf())
                return {next_vpn(order), entry.as_table_pointer()};
        }
        return {order, table};
    }

    VMMap::VMMap(PageTable *table, void *min_map_addr, void *max_map_addr)
        : m_bump_allocator(sizeof(VMMap::tree::node)), m_memmap_info_tree(m_bump_allocator), m_root_table(table),
          m_min_alloc_address(min_map_addr), m_max_alloc_address(max_map_addr) {};

    bool VMMap::is_valid_virtual_address(const void *addr)
    {
        return addr >= m_min_alloc_address && addr <= m_max_alloc_address;
    }

    void VMMap::translate_to_page(const MemMapInfo &m_map)
    {
        PageTable *p_table = m_root_table;
        for (FrameOrder c_lvl = FrameOrder::HIGHEST_ORDER;
             (c_lvl != FrameOrder::LOWEST_ORDER) && (c_lvl != m_map.get_frame_order()); c_lvl = next_vpn(c_lvl))
        {
            auto result = table_walk(m_map.get_vaddress(), p_table, c_lvl);
            // If it is true, we don't have a page table for this range of addresses
            if (result.first == c_lvl)
            {
                auto frame_info = FrameManager::get_global_instance().get_frames(1, 0);
                if (frame_info == nullptr)
                {
                    // TODO: Handle killing processes to get memory
                    PANIC("Out of memory. Can't allocate frame for page table.");
                }
                auto temp = translated_page_vaddress(p_table);
                auto &entry = temp->get_entry(get_page_entry_index(m_map.get_vaddress(), m_map.get_frame_order()));
                entry.point_to_table(reinterpret_cast<PageTable *>(frame_info->get_frame_pointer()));
                p_table = entry.as_table_pointer();
                auto vt = translated_page_vaddress(reinterpret_cast<PageTable *>(frame_info->get_frame_pointer()));
                memset(vt, 0, PAGE_FRAME_SIZE);
            }
            else
            {
                p_table = result.second;
            }
        }
        p_table = translated_page_vaddress(p_table);
        auto &entry = p_table->get_entry(get_page_entry_index(m_map.get_vaddress(), m_map.get_frame_order()));
        entry.point_to_frame(m_map.get_paddress());
        entry.set_flags(m_map.get_flags());
        flush_tlb();
    }

    MemMapInfo *VMMap::map_memory(const void *p_address, const void *v_address, FrameOrder order, uint64_t flags)
    {
        if (!is_valid_virtual_address(v_address))
            return nullptr;

        MemMapInfo temp(order, p_address, v_address, flags);
        auto check = m_memmap_info_tree.equal_or_greater(temp);
        auto pred_check = m_memmap_info_tree.get_in_order_predecessor(check);
        auto post_check = m_memmap_info_tree.get_in_order_successor(check);
        if (m_memmap_info_tree.is_valid_node(check) && temp.overlaps(check->get_data()))
            return nullptr;
        if (m_memmap_info_tree.is_valid_node(pred_check) && temp.overlaps(pred_check->get_data()))
            return nullptr;
        if (m_memmap_info_tree.is_valid_node(post_check) && temp.overlaps(post_check->get_data()))
            return nullptr;
        auto nd = m_memmap_info_tree.insert(temp);
        translate_to_page((nd->get_data()));
        return &(nd->get_data());
    }

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
