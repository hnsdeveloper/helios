#include "arch/riscv64gc/plat_def.hpp"
#include "misc/macros.hpp"
#include "misc/types.hpp"

using frame_fn = void *(*)();

namespace hls {

void *v_to_p(const void *vaddress, PageTable *table);
bool kmmap(const void *paddress, const void *vaddress, PageTable *table, PageLevel p_lvl, uint64_t flags);

} // namespace hls
