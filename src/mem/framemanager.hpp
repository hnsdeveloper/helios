#include "misc/macros.hpp"
#include "misc/types.hpp"

namespace hls {

LKERNELFUN void *get_frame();
LKERNELFUN void release_frame(void *);
LKERNELFUN void initialize_frame_manager(void *mem, size_t mem_size);

} // namespace hls