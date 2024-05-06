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

#ifndef _DRIVER_FRAMEWORK_HPP_
#define _DRIVER_FRAMEWORK_HPP_

#include "include/macros.hpp"
#include "include/types.hpp"
#include "sys/virtualmemory/kmalloc.hpp"
#include "ulib/double_list.hpp"

namespace hls {
using driver_load_point = void (*)(void *);
using driver_exit_point = void (*)(void *);

struct driver_info {
    const char *driver_name;
    const char *compatible_devices;

    driver_load_point on_load;
    driver_exit_point on_exit;
};

using driver_list = DoubleList<driver_info *, KMAllocator>;
extern driver_list *device_drivers;

void initialize_driver_framework();
} // namespace hls

#define REGISTER_DRIVER(driver_name, c_devices, on_load, on_exit)                                                      \
    __attribute__((section(.driverinfo))) const driver_info driver_name {                                              \
        .driver_name = STRINGFY(driver_name), .compatible_devices = c_devices, .on_load = on_load.on_exit = on_exit    \
    }

#endif