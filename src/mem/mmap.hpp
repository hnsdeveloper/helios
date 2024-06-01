#include "arch/riscv64gc/plat_def.hpp"
#include "misc/macros.hpp"
#include "misc/types.hpp"
#include "sys/bootdata.hpp"
#include "sys/cpu.hpp"

namespace hls
{

    void *v_to_p(const void *vaddress, PageTable *table);

    bool kmmap(const void *paddress, const void *vaddress, PageTable *table, FrameLevel p_lvl, uint64_t flags,
               frame_fn f_src);

    void kmunmap(const void *vaddress, PageTable *start_table, frame_rls_fn rls_fn);

    size_t kmmap(const void *paddress, const void *vaddress, PageTable *table, const FrameLevel p_lvl, uint64_t flags,
                 FrameKB **f_src, size_t f_count);

    void kmunmap(const void *vaddress, PageTable *ptable, FrameKB **f_dst, size_t &limit);

    uintptr_t get_vaddress_offset(const void *vaddress);

    void set_scratch_pagetable(PageTable *vpaddress);
    PageTable *get_scratch_pagetable();

    void set_kernel_pagetable(PageTable *paddress);
    PageTable *get_kernel_pagetable();

    void set_kernel_v_free_address(byte *vaddress);
    byte *get_kernel_v_free_address();

    void setup_kernel_memory_map(bootinfo *b_info);

    void unmap_low_kernel(byte *begin, byte *end);

} // namespace hls
