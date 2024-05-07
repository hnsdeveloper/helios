#include "arch/riscv64gc/plat_def.hpp"
#include "misc/macros.hpp"

#define INITIAL_PAGE_COUNT 32

using namespace hls;

LKERNELBSS GranularPage INITIAL_FRAMES[INITIAL_PAGE_COUNT];

extern "C" LKERNELFUN void bootmain() {
    memset(INITIAL_FRAMES, 0, sizeof(INITIAL_FRAMES));
}