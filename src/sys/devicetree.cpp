#include "sys/devicetree.hpp"
#include "arch/riscv64gc/plat_def.hpp"
#include "mem/framemanager.hpp"
#include "mem/mmap.hpp"
#include "misc/types.hpp"
#include "sys/mem.hpp"

namespace hls
{

    void *fdt_address;

    void mapfdt(void *fdt)
    {
        PageTable *kptp = get_kernel_pagetable();
        byte *aligned = reinterpret_cast<byte *>(align_back(fdt, PAGE_FRAME_ALIGNMENT));
        kmmap(aligned, aligned, kptp, FrameOrder::FIRST_ORDER, READ | ACCESS | DIRTY, initfalloc);

        size_t fdt_size = fdt_totalsize(fdt);
        size_t needed_size = reinterpret_cast<byte *>(fdt) - aligned + fdt_size;
        size_t needed_pages = needed_size / PAGE_FRAME_SIZE + (needed_size % PAGE_FRAME_SIZE ? 1 : 0);

        kmunmap(aligned, kptp, initffree);

        byte *addr = get_kernel_v_free_address();
        fdt_address = addr + (reinterpret_cast<byte *>(fdt) - aligned);
        for (size_t i = 0; i < needed_pages; ++i)
        {
            kmmap(aligned, addr, kptp, FrameOrder::FIRST_ORDER, READ | ACCESS | DIRTY, initfalloc);
            aligned += PAGE_FRAME_SIZE;
            addr += PAGE_FRAME_SIZE;
        }
        set_kernel_v_free_address(addr);
    }

    void *get_fdt()
    {
        return fdt_address;
    }

    reg_prop read_fdt_prop_reg_prop(void *fdt, int node, const fdt32_t *p_a_cells, const fdt32_t *p_s_cells)
    {
        reg_prop ret{.mem_address = nullptr, .mem_size = 0};
        int len;
        const fdt32_t *a_cells = reinterpret_cast<const fdt32_t *>(fdt_getprop(fdt, node, "#address-cells", &len));
        const fdt32_t *s_cells = reinterpret_cast<const fdt32_t *>(fdt_getprop(fdt, node, "#size-cells", &len));

        if (!a_cells || !s_cells)
        {
            a_cells = p_a_cells;
            s_cells = p_s_cells;
        }

        const struct fdt_property *prop = fdt_get_property(fdt, node, "reg", &len);

        const byte *prop_data = as_byte_ptr(prop->data);

        ret.mem_address = fdt32_ld(a_cells) == 1 ? to_ptr(fdt32_ld(reinterpret_cast<const fdt32_t *>(prop_data)))
                                                 : to_ptr(fdt64_ld(reinterpret_cast<const fdt64_t *>(prop_data)));
        prop_data += sizeof(fdt32_t) * (fdt32_ld(a_cells));
        ret.mem_size = fdt32_ld(s_cells) == 1 ? fdt32_ld(reinterpret_cast<const fdt32_t *>(prop_data))
                                              : fdt64_ld(reinterpret_cast<const fdt64_t *>(prop_data));

        return ret;
    }

} // namespace hls
