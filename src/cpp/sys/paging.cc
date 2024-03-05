export module Paging;

#include "arch/riscv/plat_def.h"
#include "global.h"
#include "symbols.h"

export namespace hls {

class PageFrameManager;

void setup_paging();

} // namespace hls