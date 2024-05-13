/*---------------------------------------------------------------------------------
MIT License

Copyright (c) 2024 Helio Nunes Santos

        Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
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

#include "arch/riscv64gc/plat_def.hpp"
#include "misc/macros.hpp"
#include "misc/types.hpp"
#include "sys/bootdata.hpp"

#define FRAME_SWAPPABLE 1 << 0

namespace hls {

struct frame_info {
    FrameKB *frame_pointer = nullptr;
    size_t frame_count = 0;
    size_t use_count = 0;
    uint64_t flags = 0;

    size_t size() {
        return FrameKB::s_size * frame_count;
    }
};

const frame_info *get_frame(uint64_t flags);
void release_frame(void *);

const frame_info *get_frames(size_t count, uint64_t flags);
void *release_frames(void *);

void *get_frame_management_begin_vaddress();
void *get_frame_management_end_vaddress();

void initialize_frame_manager(void *fdt, bootinfo *b_info);

} // namespace hls