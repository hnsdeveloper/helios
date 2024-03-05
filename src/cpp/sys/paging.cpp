module Paging;

import Memory;
import Bit;

namespace hls {

class PageFrameManager {
  PageFrame *frames = nullptr; // A pointer to the first frame
  Bit<64> *m_bitmap;
  size_t m_frame_count;

public:
  PageFrameManager(void *base_address, size_t mem_size) m_bitmap =
      reinterpret_cast<Bit<64> *>(align(base_address, alignof(Bit<64>)));

}

~PageFrameManager() {
}

void free_frame(PageFrame *frame) {}

Expected<PageFrame *> get_frame() {}
};

} // namespace hls
