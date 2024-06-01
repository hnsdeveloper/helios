#include "sys/panic.hpp"
#include "sys/print.hpp"

using namespace hls;

void print_stack_trace(void *address, size_t level)
{
    auto print_space = [](size_t level) {
        for (size_t i = 0; i < level; ++i)
        {
            kprint(" ");
        }
    };

    if (level)
    {
        print_space(level);
        kprintln("|");
        print_space(level);
        kprint(">");
    }

    kprintln(" Called from {}.", address);
}

extern "C" void print_registers(registers *h)
{
    for (size_t i = 0; i < 32; ++i)
    {
        kprint("x{}: {} ", i, reinterpret_cast<void *>(h->reg.array[i]));
        if ((i + 1) % 4 == 0)
        {
            strprintln("");
        }
    }
    kprintln("pc: {}", reinterpret_cast<void *>(h->reg.data.pc));
}

extern "C" void stack_trace()
{
    void *fp = nullptr;
    size_t level = 0;

    // Lambda to read the return address from the frame pointer
    auto read_ra = [](void *fp) {
        uint64_t *v = reinterpret_cast<uint64_t *>(fp);
        return reinterpret_cast<void *>(*(v - 1) - 4);
    };

    // Lambda to read the next frame pointer in the chain
    auto read_fp = [](void *fp) {
        uint64_t *v = reinterpret_cast<uint64_t *>(fp);
        return reinterpret_cast<void *>(*(v - 2));
    };

    asm("add %0, x0, s0;" : "=r"(fp) : "r"(fp));

    // The first one we skip, given that it will point to the call to
    // stack_trace
    fp = read_fp(fp);

    while (fp != nullptr)
    {
        print_stack_trace(read_ra(fp), level);
        ++level;
        fp = read_fp(fp);
    }
}

/**
 * @todo Implement properly
 *
 */
extern "C" void die()
{
    kprintln("Please, manually reboot the machine.");
    while (true)
        ;
}

extern "C" void panic_message_print(const char *msg)
{
    kprintln(msg);
}
