#include "arch/riscv64gc/plat_def.hpp"
#include "mem/mmap.hpp"
#include "misc/macros.hpp"
#include "misc/symbols.hpp"
#include "sys/opensbi.hpp"

#define INITIAL_PAGE_COUNT 32

using namespace hls;

LKERNELFUN void strprintln(const char *str);

LKERNELBSS GranularPage INITIAL_FRAMES[INITIAL_PAGE_COUNT];

LKERNELRODATA const char BOOTING_STRING[] = "Booting HELIOS!";
LKERNELRODATA const char NEW_LINE[] = "\r\n";

const char *TESTLINE = "\r\n";

LKERNELFUN void init_f_alloc() {
    memset(INITIAL_FRAMES, 0, sizeof(INITIAL_FRAMES));
}

LKERNELFUN void *f_alloc() {
    LKERNELDATA static size_t i = 0;
    if (i < INITIAL_PAGE_COUNT)
        return &INITIAL_FRAMES[i++];
    return nullptr;
}

LKERNELFUN void strprint(const char *str) {
    while (*str) {
        opensbi_putchar(*str);
        ++str;
    }
}

LKERNELFUN void strprintln(const char *str) {
    strprint(str);
    strprint(NEW_LINE);
}

LKERNELFUN void map_kernel(PageTable *kernel_table) {
    uintptr_t k_address = 0xFFFFFFFFBFFFFFFF;

    byte *k_vaddress = reinterpret_cast<byte *>(to_ptr(k_address));
    byte *text_begin = &_text_begin;
    byte *text_end = &_text_end;

    for (auto p = text_begin; p < text_end; p += PAGE_FRAME_SIZE) {
        hls::kmmap(text_begin, k_vaddress, kernel_table, PageLevel::KB_VPN, READ | EXECUTE | ACCESS | DIRTY, f_alloc);
    }
}

extern "C" LKERNELFUN void *bootmain(int argc, char **argv) {
    strprintln(BOOTING_STRING);
    init_f_alloc();
    PageTable *kernel_table = reinterpret_cast<PageTable *>(f_alloc());
    map_kernel(kernel_table);

    return kernel_table;
}