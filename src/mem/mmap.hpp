#include "arch/riscv64gc/plat_def.hpp"
#include "misc/macros.hpp"
#include "misc/types.hpp"
#include "sys/cpu.hpp"

namespace hls {

void *v_to_p(const void *vaddress, PageTable *table);
bool kmmap(const void *paddress, const void *vaddress, PageTable *table, PageLevel p_lvl, uint64_t flags,
           frame_fn f_src);

void set_scratch_page(PageTable *vpaddress);
PageTable *get_scratch_page();

} // namespace hls
