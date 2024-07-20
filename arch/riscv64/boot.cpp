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

#include "plat_def.hpp"
#include "misc/macros.hpp"
#include "misc/symbols.hpp"
#include "sys/bootdata.hpp"

#ifndef BOOTPAGES
#define BOOTPAGES 64
#endif

#ifndef ARGPAGES
#define ARGPAGES 2
#endif

using namespace hls;

LKERNELBSS alignas(PAGE_FRAME_SIZE) byte INITIAL_FRAMES[FrameKB::s_size * BOOTPAGES];
LKERNELBSS alignas(PAGE_FRAME_SIZE) byte ARGCV[FrameKB::s_size * ARGPAGES];
LKERNELDATA static size_t s_used = 0;
LKERNELRODATA const char NEEDPAGES[] = "Not enough pages. Please, compile kernel with higher BOOTPAGES option.";
LKERNELRODATA const char NEEDARGCV[] =
    "Not enough pages for arguments. Please, compile kernel with higher ARGPAGES option.";
LKERNELDATA byte *kvaddress = reinterpret_cast<byte *>(uintptr_t(0) - uintptr_t(0x40000000));

LKERNELFUN void _sbi_call(uint64_t extension, uint64_t function_id, uint64_t arg0, uint64_t arg1, uint64_t arg2,
                          uint64_t arg3, uint64_t arg4, uint64_t arg5)
{
    register uint64_t a0 asm("a0") = arg0;
    register uint64_t a1 asm("a1") = arg1;
    register uint64_t a2 asm("a2") = arg2;
    register uint64_t a3 asm("a3") = arg3;
    register uint64_t a4 asm("a4") = arg4;
    register uint64_t a5 asm("a5") = arg5;
    register uint64_t a6 asm("a6") = function_id;
    register uint64_t a7 asm("a7") = extension;
    asm volatile("ecall" : "+r"(a0), "+r"(a1) : "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a6), "r"(a7) : "memory");
}

LKERNELFUN void bputc(char c)
{
    _sbi_call(0x1u, 0x0, c, 0, 0, 0, 0, 0);
}

LKERNELFUN void bputstr(const char *str)
{
    while (str && *str)
        bputc(*(str++));
}

LKERNELFUN void bputstrln(const char *str)
{
    bputstr(str);
    bputc('\n');
}

LKERNELFUN void *bmemset(void *mem, byte data, size_t size)
{
    byte *m = reinterpret_cast<byte *>(mem);

    for (; size; --size)
        *(m++) = data;

    return mem;
}

LKERNELFUN void bmemcpy(void *dest, const void *src, size_t bytes)
{
    const byte *src_c = reinterpret_cast<const byte *>(src);
    byte *dest_c = reinterpret_cast<byte *>(dest);
    while (bytes--)
    {
        *dest_c = *src_c;
        ++src_c;
        ++dest_c;
    }
}

LKERNELFUN size_t bstrlen(const char *a)
{
    size_t i = 0;
    while (a && *(a++))
        ++i;
    return i;
}

LKERNELFUN void *bptr(uintptr_t p)
{
    return reinterpret_cast<void *>(p);
}

LKERNELFUN uintptr_t buintptr_t(const void *p)
{
    return reinterpret_cast<uintptr_t>(const_cast<void *>(p));
}

LKERNELFUN FrameOrder nvpn(FrameOrder v)
{
    return v == FrameOrder::FIRST_ORDER ? FrameOrder::FIRST_ORDER : static_cast<FrameOrder>(static_cast<size_t>(v) - 1);
}

LKERNELFUN size_t pe_idx(const void *vaddress, FrameOrder v)
{
    size_t vpn_idx = static_cast<size_t>(v);
    uintptr_t idx = buintptr_t(vaddress) >> 12;
    return (idx >> (vpn_idx * 9)) & 0x1FF;
}

LKERNELFUN void twlk(const void *vaddress, PageTable **table, FrameOrder *lvl)
{
    if (table == nullptr || lvl == nullptr)
        return;

    FrameOrder l = *lvl;
    PageTable *t = *table;
    size_t idx = pe_idx(vaddress, l);
    auto &entry = reinterpret_cast<TableEntry *>(t)[idx];

    if (entry.data & VALID)
    {
        if (!(entry.data & (READ | WRITE | EXECUTE)))
        {
            *table = reinterpret_cast<PageTable *>((entry.data >> 10) << 12);
            *lvl = nvpn(*lvl);
        }
    }
}

LKERNELFUN void init_f_alloc()
{
    bmemset(INITIAL_FRAMES, 0, sizeof(INITIAL_FRAMES));
    s_used = 0;
}

LKERNELFUN void *f_alloc()
{

    FrameKB *p = reinterpret_cast<FrameKB *>(INITIAL_FRAMES);
    if (s_used < BOOTPAGES)
        return p + s_used++;

    bputstrln(NEEDPAGES);
    while (true)
        ;

    return nullptr;
}

LKERNELFUN PageTable *bkmmap(const void *paddress, const void *vaddress, PageTable *table, const FrameOrder p_lvl,
                             uint64_t flags)
{

    if (table == nullptr)
        return nullptr;

    FrameOrder c_lvl = FrameOrder::HIGHEST_ORDER;
    FrameOrder expected = nvpn(c_lvl);
    PageTable *t = table;

    while (c_lvl != p_lvl)
    {
        twlk(vaddress, &t, &c_lvl);
        if (c_lvl == expected)
        {
            expected = nvpn(expected);
            continue;
        }

        size_t idx = pe_idx(vaddress, c_lvl);
        auto &entry = reinterpret_cast<TableEntry *>(t)[idx];

        if (entry.data & (READ | WRITE | EXECUTE))
            return nullptr;

        if (!(entry.data & VALID))
        {
            void *frame = f_alloc();
            // We can use this memory without zeroing it.
            entry.data = ((reinterpret_cast<uint64_t>(frame) >> 12) << 10);
            entry.data = entry.data | VALID;
        }

        c_lvl = nvpn(c_lvl);
        expected = nvpn(expected);
        t = reinterpret_cast<PageTable *>(bptr((entry.data >> 10) << 12));
    }

    size_t lvl_entry_idx = pe_idx(vaddress, c_lvl);
    auto &entry = reinterpret_cast<TableEntry *>(t)[lvl_entry_idx];
    entry.data = (buintptr_t(paddress) >> 12) << 10;
    entry.data = entry.data | VALID | flags;

    return t;
}

LKERNELFUN void *map_high_kernel(PageTable *kernel_table)
{
    byte *_k_physical = &_kload_begin;

    kvaddress = &_text_begin;
    for (; kvaddress != &_text_end; kvaddress += PAGE_FRAME_SIZE, _k_physical += PAGE_FRAME_SIZE)
    {
        bkmmap(_k_physical, kvaddress, kernel_table, FrameOrder::LOWEST_ORDER, READ | EXECUTE | ACCESS | DIRTY);
    }
    kvaddress = &_rodata_begin;
    for (; kvaddress != &_rodata_end; kvaddress += PAGE_FRAME_SIZE, _k_physical += PAGE_FRAME_SIZE)
    {
        bkmmap(_k_physical, kvaddress, kernel_table, FrameOrder::LOWEST_ORDER, READ | ACCESS | DIRTY);
    }

    kvaddress = &_data_begin;
    // DATA, BSS and STACK are all READ and WRITE
    for (; kvaddress != &_stack_end; kvaddress += PAGE_FRAME_SIZE, _k_physical += PAGE_FRAME_SIZE)
    {
        bkmmap(_k_physical, kvaddress, kernel_table, FrameOrder::LOWEST_ORDER, READ | WRITE | ACCESS | DIRTY);
    }

    return _k_physical;
}

LKERNELFUN void identity_map(PageTable *kernel_table)
{
    for (auto i = &_load_address; i != &_kload_begin; i += PAGE_FRAME_SIZE)
    {
        bkmmap(i, i, kernel_table, FrameOrder::LOWEST_ORDER, READ | WRITE | EXECUTE | ACCESS | DIRTY);
    }
}

LKERNELFUN const char **map_args(PageTable *kernel_table, int argc, const char **argv)
{
    size_t consumed_bytes = 0;
    byte *c = ARGCV;
    const char *nargv[argc];
    for (size_t i = 0; i < (size_t)(argc); ++i)
    {
        const char *str = argv[i];
        // Memory length, not string length
        size_t len = bstrlen(str) + 1;
        bmemcpy(c, str, len);
        nargv[i] = reinterpret_cast<const char *>(kvaddress + consumed_bytes);
        consumed_bytes += len;
        c += len;
        if (consumed_bytes > sizeof(ARGCV))
        {
            bputstrln(NEEDARGCV);
            while (true)
                ;
        }
    }

    if (consumed_bytes + sizeof(nargv) > sizeof(ARGCV))
    {
        bputstrln(NEEDARGCV);
        while (true)
            ;
    }
    bmemcpy(c, &nargv, sizeof(nargv));

    auto old_kv = kvaddress;
    for (size_t i = 0; i < ARGPAGES; ++i)
    {
        bkmmap(ARGCV + i * PAGE_FRAME_SIZE, kvaddress, kernel_table, FrameOrder::FIRST_ORDER, READ | ACCESS | DIRTY);
        kvaddress += PAGE_FRAME_SIZE;
    }

    return reinterpret_cast<const char **>(old_kv + consumed_bytes);
}

LKERNELFUN PageTable *force_scratch_page(PageTable *kernel_table)
{
    byte *p = nullptr;
    p = p - PAGE_FRAME_SIZE;

    auto t = bkmmap(p, p, kernel_table, FrameOrder::FIRST_ORDER, READ | WRITE);
    bkmmap(t, p, kernel_table, FrameOrder::FIRST_ORDER, READ | WRITE);

    return reinterpret_cast<PageTable *>(p);
}

extern "C" LKERNELFUN void bootmain(int argc, const char **argv, bootinfo *info)
{
    init_f_alloc();
    PageTable *kernel_table = reinterpret_cast<PageTable *>(f_alloc());
    auto *k_ph_end = map_high_kernel(kernel_table);
    auto scratch = force_scratch_page(kernel_table);
    identity_map(kernel_table);
    auto nargv = map_args(kernel_table, argc, argv);
    info->argc = argc;
    info->argv = nargv;
    info->used_bootpages = s_used;
    info->p_kernel_table = kernel_table;
    info->v_scratch = scratch;
    info->v_highkernel_start = &_kload_begin;
    info->v_highkernel_end = kvaddress;
    info->p_lowkernel_start = &_load_address;
    info->p_lowkernel_end = &_kload_begin;
    info->p_kernel_physical_end = reinterpret_cast<byte *>(k_ph_end);
    info->v_device_drivers_begin = &_driverinfo_begin;
    info->v_device_drivers_end = &_driverinfo_end;
}
