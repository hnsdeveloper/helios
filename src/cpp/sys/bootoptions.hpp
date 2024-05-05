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

#ifndef _BOOT_OPTIONS_HPP_
#define _BOOT_OPTIONS_HPP_

#include "include/types.hpp"
#include "misc/leanmeanparser/optionparser.hpp"
#include "sys/print.hpp"
#include "sys/string.hpp"

namespace hls {

struct ArgVal : public option::Arg {
    static option::ArgStatus NumericHex(const option::Option &option, bool) {
        if (option.arg == nullptr) {
            return option::ARG_ILLEGAL;
        }

        char *p = nullptr;
        size_t val = strtoul(option.arg, &p, 16);
        if (val == 0 && p == nullptr) {

            return option::ARG_ILLEGAL;
        }

        return option::ARG_OK;
    }

    static option::ArgStatus Unique(const option::Option &option, bool) {

        if (option.count() != 1) {
            return option::ARG_ILLEGAL;
        }

        return option::ARG_OK;
    }

    static option::ArgStatus NumericHexUnique(const option::Option &option, bool msg) {
        if (Unique(option, msg) == option::ARG_OK && NumericHex(option, msg) == option::ARG_OK)
            return option::ARG_OK;

        return option::ARG_ILLEGAL;
    }
};

enum OptionIndex {
    UNKNOWN,
    HELP,
    FDT
};

const option::Descriptor usage[] = {{UNKNOWN, 0, "", "", ArgVal::None, "USAGE: example [options]\n\nOptions:"},
                                    {HELP, 0, "h", "help", ArgVal::None, "  --help    \tPrint usage and exit."},
                                    {FDT, 0, "f", "fdt", ArgVal::NumericHexUnique,
                                     "  --fdt, -f \t Flattened device tree address as hex value (e.g. FFFFFFFF "
                                     "or 0xFFFFFFF)."}};

} // namespace hls

#endif