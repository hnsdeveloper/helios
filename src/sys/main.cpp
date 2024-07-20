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

#include "leanmeanparser/optionparser.hpp"
#include "mem/framemanager.hpp"
#include "mem/mmap.hpp"
#include "misc/githash.hpp"
#include "misc/splash.hpp"
#include "misc/symbols.hpp"
#include "plat_def.hpp"
#include "sys/bootdata.hpp"
#include "sys/bootoptions.hpp"
#include "sys/cpu.hpp"
#include "sys/devicetree.hpp"
#include "sys/kmalloc.hpp"
#include "sys/mem.hpp"
#include "sys/print.hpp"
#include "sys/string.hpp"

namespace hls
{

    void display_initial_info()
    {
        // Prints the splash logo
        strprint(splash);
        // Prints copyright notice, the year and the commit which this build was based
        // at.
        kprintln("Copyright (C) {}. Built from {}.", __DATE__ + 7, GIT_HASH);
    }

    void unmap_low_kernel(byte *begin, byte *end)
    {
        for (auto it = begin; it < end; it += PAGE_FRAME_SIZE)
        {
            VMMap::get_global_instance().unmap_memory(it);
        }
    }

    __attribute__((noreturn)) void kernel_main(bootinfo *b_info)
    {
        display_initial_info();

        // Initialize FrameManager with pages that remain from the bootstage.
        const Pair<void *, size_t> boot_pages{b_info->p_kernel_table + b_info->used_bootpages,
                                              (BOOTPAGES - b_info->used_bootpages) * FrameKB::s_size};
        FrameManager::initialize_global_instance();
        FrameManager::get_global_instance().expand_memory(boot_pages);

        // Initialize kernel memory mapper and unmap low kernel, given that we don't rely on it anymore.
        VMMap::initialize_global_instance(b_info->p_kernel_table, b_info->v_scratch);
        unmap_low_kernel(b_info->p_lowkernel_start, b_info->p_lowkernel_end);
        kprintln("Here!");

        while (true)
            ;

        mapfdt(get_device_tree_from_options(b_info->argc, b_info->argv));
        initialize_frame_manager(get_fdt(), b_info);

        // initialize_kmalloc();
        while (true)
            ;
    }

}; // namespace hls

extern "C" void _main(hls::bootinfo *info)
{
    auto info_cp = *info;
    hls::kernel_main(&info_cp);
}
