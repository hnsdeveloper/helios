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

#include "sys/bootoptions.hpp"
#include "sys/cpu.hpp"

namespace hls
{

    void strcprint(const char *str, size_t n)
    {
        while (n--)
        {
            opensbi_putchar(*(str++));
        }
    }

    void *get_device_tree_from_options(int argc, const char **argv)
    {
        option::Stats stats(usage, argc - 1, argv + 1);
        option::Option options[stats.options_max], buffer[stats.buffer_max];
        option::Parser parse(usage, argc - 1, argv + 1, options, buffer);

        if (parse.error())
        {
            kprintln("Failed to parse boot options.");
            die();
        }

        if (argc == 1 || options[OptionIndex::HELP])
        {
            option::printUsage(&strcprint, usage);
            die();
        }

        if (options[OptionIndex::FDT].count() == 1)
        {
            char *p = nullptr;
            uintptr_t addr = strtoul(options[OptionIndex::FDT].arg, &p, 16);

            if (addr == 0 && p == nullptr)
            {
                kprintln("Invalid FDT address. Please reboot and provide a valid one (FDT needed for booting).");
                die();
            }

            return to_ptr(addr);
        }
        else
        {
            kprintln("Invalid fdt option.");
        }

        return nullptr;
    }
} // namespace hls