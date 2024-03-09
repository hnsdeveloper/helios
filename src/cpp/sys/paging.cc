export module Paging;

#include "global.h"
#include "symbols.h"

struct PageTable;
struct PageEntry;
struct PageFrame;

export namespace hls {

class PageFrameManager;

void setup_paging();

PageTable *get_kernel_page_table();

} // namespace hls