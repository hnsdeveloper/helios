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

#include "sys/memmap.hpp"
#include "include/arch/riscv/plat_def.h"
#include "sys/mem.hpp"
#include "sys/paging.hpp"
#include "sys/print.hpp"
#include "ulib/expected.hpp"

namespace hls {

void setup_memory_mapping() {
  auto &frame_manager = PageFrameManager::instance();
  PageFrame *frames[2];

  for (size_t i = 0; i < 2; ++i) {
    auto result = frame_manager.get_frame();
    if (result.is_error())
      print("Error while acquiring page frames. Error code: ")(
          result.get_error())("\r\n");

    frames[i] = result.get_value();
    memset(&frames[i], 0, sizeof(PageFrame));
  }

  PageTable *table = reinterpret_cast<PageTable *>(frames[0]);
  table->get_entry(0).point_to_frame(frames[1]);

  table = reinterpret_cast<PageTable *>(frames[1]);
  auto &first = table->get_entry(0);
  auto &second = table->get_entry(1);

  // We are mapping now from 0x00000000 to 0x800000000
  // This is to QEMU Risc V platform
  // TODO: Change the code to generalize it

  char *address = 0;

  first.point_to_frame(reinterpret_cast<PageFrame *>(address));
  first.make_readable(true);
  first.make_writable(true);
  first.make_executable(true);

  address = address + 0xC0000000;

  second.point_to_frame(reinterpret_cast<PageFrame *>(address));
  second.make_readable(true);
  second.make_writable(true);
  second.make_executable(true);
}

} // namespace hls