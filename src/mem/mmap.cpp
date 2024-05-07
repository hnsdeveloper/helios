#include "mmap.hpp"
#include "framemanager.hpp"

namespace hls {

LKERNELFUN size_t get_page_entry_index(const void *vaddress, PageLevel v) {
    size_t vpn_idx = static_cast<size_t>(v);
    uintptr_t idx = to_uintptr_t(vaddress) >> 12;
    return (idx >> (vpn_idx * 9)) & 0x1FF;
}

LKERNELFUN uintptr_t get_vaddress_offset(const void *vaddress) {
    uintptr_t p = to_uintptr_t(vaddress);
    return p & 0xFFF;
}

LKERNELFUN void walk_table(const void *vaddress, PageTable **table_ptr, PageLevel &page_level) {
    PageTable *table = *table_ptr;

    if (table == nullptr || page_level == PageLevel::FIRST_VPN)
        return;

    uintptr_t vpn = get_page_entry_index(vaddress, page_level);
    auto &entry = table->get_entry(vpn);

    if (entry.is_valid() && !entry.is_leaf()) {
        *table_ptr = entry.as_table_pointer();
        page_level = next_vpn(page_level);
    }
}

LKERNELFUN void *v_to_p(const void *vaddress, PageTable *table = nullptr) {
    if (table != nullptr) {

        constexpr size_t p_lvl_count = (size_t)(PageLevel::LAST_VPN);
        PageLevel lvl = PageLevel::LAST_VPN;
        for (size_t i = 0; i < p_lvl_count; ++i) {
            // Will walk the table as much as it can.
            // If it is a superpage, then it won't walk more, and then we only apply the offset to the address
            walk_table(vaddress, &table, lvl);
        }

        auto &entry = table->get_entry(get_page_entry_index(vaddress, lvl));

        if (entry.is_valid() && entry.is_leaf()) {
            return apply_offset(entry.as_pointer(), get_vaddress_offset(vaddress));
        }
    }
    return nullptr;
}

LKERNELFUN bool kmmap(const void *paddress, const void *vaddress, PageTable *table, PageLevel p_lvl, uint64_t flags,
                      frame_fn f_src) {

    if (f_src == nullptr)
        f_src = &get_frame;
    void *p = v_to_p(vaddress);

    if (p != nullptr)
        return false;

    if (table == nullptr)
        return false;

    PageLevel c_lvl = PageLevel::LAST_VPN;
    PageLevel expected = next_vpn(c_lvl);
    PageTable *t = table;

    while (c_lvl != p_lvl) {
        walk_table(vaddress, &t, c_lvl);
        if (c_lvl != expected) {
            size_t lvl_entry_idx = get_page_entry_index(vaddress, c_lvl);
            auto &entry = t->get_entry(lvl_entry_idx);
            if (!entry.is_valid()) {
                PageTable *n_table = reinterpret_cast<PageTable *>(f_src());
                // TODO: HANDLE ERROR (OUT OF MEMORY)
                if (n_table == nullptr)
                    return false;
                memset(n_table, 0, PAGE_FRAME_SIZE);
                entry.point_to_table(n_table);
            }
        }
    }

    size_t lvl_entry_idx = get_page_entry_index(vaddress, c_lvl);
    auto &entry = t->get_entry(lvl_entry_idx);
    entry.point_to_frame(paddress);
    entry.set_flags(flags);

    return true;
}

} // namespace hls