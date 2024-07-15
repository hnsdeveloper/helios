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

    Pair<FrameOrder, PageTable *> VMMap::table_walk(const void *vaddress, PageTable *table, FrameOrder order)
    {
        if (table != nullptr && order != FrameOrder::LOWEST_ORDER)
        {
            size_t idx = get_page_entry_index(vaddress, order);
            auto v_table = reinterpret_cast<PageTable *>(physical_frame_to_scratch_frame(table));
            auto &entry = v_table->get_entry(idx);
            if (entry.is_valid() && !entry.is_leaf())
                return {next_vpn(order), entry.as_table_pointer()};
        }
        return {order, table};
    }

    PageTable *VMMap::get_scratch_table()
    {
        return m_v_scratch_table;
    }

    FrameKB *VMMap::physical_frame_to_scratch_frame(FrameKB *frame)
    {
        auto scratch = get_scratch_table();
        auto &entry = scratch->get_entry(PageTable::entries_on_table - get_cpu_id() - 2);
        entry.point_to_frame(frame);
        entry.set_system_flags(VM_READ_FLAG | VM_WRITE_FLAG | VM_ACCESS_FLAG | VM_DIRTY_FLAG);
        flush_tlb();
        return (FrameKB *)(nullptr) - get_cpu_id() - 2;
    }

    VMMap::VMMap(PageTable *table, PageTable *scratch_table)
        : m_p_root_table(table), m_v_scratch_table(scratch_table) {};

    bool VMMap::is_valid_virtual_address(const void *addr)
    {
        (void)(addr);
        return true;
        // return addr >= m_min_alloc_address && addr <= m_max_alloc_address;
    }

    bool VMMap::is_address_mapped(const void *vaddress)
    {
        FrameOrder c_lvl = FrameOrder::HIGHEST_ORDER;
        PageTable *table = m_p_root_table;
        do
        {
            PageTable *vtable = reinterpret_cast<PageTable *>(physical_frame_to_scratch_frame(table));
            size_t idx = get_page_entry_index(vaddress, c_lvl);
            auto &entry = vtable->get_entry(idx);
            // TODO: Handle mapped but not present table/frame
            if (!entry.is_valid())
                break;
            if (entry.is_valid() && entry.is_leaf())
                return true;
            table = entry.as_table_pointer();
            c_lvl = next_vpn(c_lvl);
        } while (true);

        return false;
    }

    Result<MemMapInfo> VMMap::map_memory(void *paddress, void *vaddress, FrameOrder order, uint64_t flags)
    {
        MemMapInfo m_map{order, paddress, vaddress, flags};

        if (is_address_mapped(vaddress))
            return error<MemMapInfo>(Error::ADDRESS_ALREADY_MAPPED);
        if (!is_valid_virtual_address(vaddress))
            return error<MemMapInfo>(Error::INVALID_VIRTUAL_ADDRESS);
        if (!is_aligned(paddress, get_frame_alignment(order)) || !is_aligned(vaddress, get_frame_alignment(order)))
            return error<MemMapInfo>(Error::MISALIGNED_MEMORY_ADDRESS);

        PageTable *p_table = m_p_root_table;
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
                    // TODO: Handle freeing memory
                    PANIC("Out of memory. Can't allocate frame for page table.");
                }
                auto temp = reinterpret_cast<PageTable *>(physical_frame_to_scratch_frame(p_table));
                auto &entry = temp->get_entry(get_page_entry_index(m_map.get_vaddress(), m_map.get_frame_order()));
                entry.point_to_table(reinterpret_cast<PageTable *>(frame_info->get_frame_pointer()));
                p_table = entry.as_table_pointer();
                auto vt = reinterpret_cast<PageTable *>(
                    physical_frame_to_scratch_frame(reinterpret_cast<FrameKB *>(frame_info->get_frame_pointer())));
                memset(vt, 0, FrameKB::s_size);
            }
            else
            {
                p_table = result.second;
            }
        }
        p_table = reinterpret_cast<PageTable *>(physical_frame_to_scratch_frame(p_table));
        auto &entry = p_table->get_entry(get_page_entry_index(m_map.get_vaddress(), m_map.get_frame_order()));
        entry.point_to_frame(m_map.get_paddress());
        entry.set_system_flags(m_map.get_flags());
        flush_tlb();
        return value(m_map);
    }

    Result<MemMapInfo> VMMap::map_first_fit(void *paddress, FrameOrder order, uint64_t flags)
    {
        (void)(paddress);
        (void)(order);
        (void)(flags);
        return error<MemMapInfo>(Error::UNDEFINED_ERROR);
    }

    void VMMap::unmap_memory(void *vaddress)
    {
        if (!is_address_mapped(vaddress))
            return;
        constexpr size_t tables = static_cast<size_t>(FrameOrder::HIGHEST_ORDER) + 1;
        PageTable *table_path[tables];
        size_t i = 0;
        FrameOrder order = FrameOrder::HIGHEST_ORDER;
        PageTable *table = m_p_root_table;
        TableEntry *entry = nullptr;
        do
        {
            table_path[i++] = table;
            PageTable *vtable = reinterpret_cast<PageTable *>(physical_frame_to_scratch_frame(table));
            size_t idx = get_page_entry_index(vaddress, order);
            entry = &(vtable->get_entry(idx));
            table = entry->as_table_pointer();
            order = next_vpn(order);
        } while (!entry->is_leaf());

        bool erased_last = false;
        do
        {
            table = table_path[--i];
            PageTable *vtable = reinterpret_cast<PageTable *>(physical_frame_to_scratch_frame(table));
            size_t idx = get_page_entry_index(vaddress, order);
            entry = &(vtable->get_entry(idx));
            if (entry->is_leaf())
            {
                entry->erase();
                if (vtable->is_empty())
                {
                    FrameManager::get_global_instance().release_frames(table);
                    erased_last = true;
                }
            }
            else if (erased_last)
            {
                entry->erase();
                if (vtable->is_empty())
                    FrameManager::get_global_instance().release_frames(table);
                else
                    erased_last = false;
            }
        } while (i != 0);

        // TODO: Check if this is kernel page table and if so, do a TLB shootdown.
        flush_tlb();
    }
} // namespace hls
