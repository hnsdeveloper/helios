#include "sys/devicetree.hpp"
#include "arch/riscv64gc/plat_def.hpp"
#include "mem/framemanager.hpp"
#include "mem/mmap.hpp"
#include "misc/libfdt/libfdt.h"
#include "misc/types.hpp"
#include "sys/mem.hpp"

namespace hls
{

    void *fdt_address;

    void mapfdt(void *fdt)
    {
        PageTable *kptp = get_kernel_pagetable();
        byte *aligned = reinterpret_cast<byte *>(align_back(fdt, PAGE_FRAME_ALIGNMENT));
        kmmap(aligned, aligned, kptp, FrameLevel::KB_VPN, READ | ACCESS | DIRTY, initfalloc);

        size_t fdt_size = fdt_totalsize(fdt);
        size_t needed_size = reinterpret_cast<byte *>(fdt) - aligned + fdt_size;
        size_t needed_pages = needed_size / PAGE_FRAME_SIZE + (needed_size % PAGE_FRAME_SIZE ? 1 : 0);

        kmunmap(aligned, kptp, initffree);

        byte *addr = get_kernel_v_free_address();
        fdt_address = addr + (reinterpret_cast<byte *>(fdt) - aligned);
        for (size_t i = 0; i < needed_pages; ++i)
        {
            kmmap(aligned, addr, kptp, FrameLevel::KB_VPN, READ | ACCESS | DIRTY, initfalloc);
            aligned += PAGE_FRAME_SIZE;
            addr += PAGE_FRAME_SIZE;
        }
        set_kernel_v_free_address(addr);
    }

    void *get_fdt()
    {
        return fdt_address;
    }

} // namespace hls
