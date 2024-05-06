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
#ifndef _PRINT_HPP_
#define _PRINT_HPP_

#include "misc/types.hpp"
#include "misc/typetraits.hpp"

extern "C" putchar_func_ptr putchar;

namespace hls {

/**
 * @brief Set the up printing function for printing to a default console.
 * @remark Thread safety: ST.
 */
void setup_printing();

/**
 * @brief Prints up to n characters from string str
 *
 * @param str The string to be printed.
 * @param n Amount of characters.
 */
void strcprint(const char *str, size_t n);

/**
 * @brief Prints string **str** to default console without formatting.
 * @remark Thread safety: ST.
 * @param str String to be printed.
 */
void strprint(const char *str);

/**
 * @brief Same as strprint, but inserts new line at the end.
 * @remark Thread safety: ST.
 * @param str String to be printed.
 */
void strprintln(const char *str);

/**
 * @brief Prints a pointer value to default console as a hex integral.
 * @remark Thread safety: ST.
 * @param p Pointer value to be printed.
 */
void ptrprint(const void *p);

/**
 * @brief Prints a signed integral value to default console.
 * @remark Thread safety: ST.
 * @param v Value to be printed.
 */
void intprint(int64_t v);

/**
 * @brief Prints an unsigned integral value to default console.
 * @remark Thread safety: ST.
 * @param v Value to be printed.
 */
void uintprint(uint64_t v);

/**
 * @brief Prints a float value to default console.
 * @remark Thread safety: ST.
 * @todo Implement
 * @param d The value to be printed
 */
void floatprint(double d);

/**
 * @brief Prints string with formatting (to default console), using the pair {}
 * as placeholder for arguments. Supports printing other strings, pointers and
 * integral values. Note that printing a const char* will print it as a string,
 * not as a pointer. It is safe in the sense that it will not read garbage from
 * the stack if there is a mismatch of placeholders and arguments.
 * Should generally not be used directly.
 *
 * @remark Thread safety: ST.
 * @tparam T The type of a first {} argument.
 * @tparam Args The types of the remaining arguments if any
 * @param str String containing formatting placeholders.
 * @param p The argument to be formatted into the string.
 * @param args The remaining arguments to be formated into the string.
 */
template <typename T, typename... Args> void print_v(const char *str, T p, Args... args) {
    if (str == nullptr) {
        strprint("Can't print nullptr string.");
    }

    while (*str) {
        auto c = *str;
        if (c == '{') {
            if (*(str + 1) != '}') {
                strprint("{");
                print_v(str + 1, p, args...);
                break;
            }
            using type = decltype(p);
            if constexpr (is_same_v<type, const char *>) {
                strprint(p);
            } else if constexpr (is_same_v<remove_cvref_t<type>, char>) {
                putchar(p);
            } else if constexpr (is_pointer_v<type>) {
                ptrprint(p);
            } else if constexpr (is_integral_v<type>) {
                if (is_signed<type>::value) {
                    intprint(p);
                } else if constexpr (!is_signed<type>::value) {
                    uintprint(p);
                }
            } else if constexpr (is_floating_point_v<type>) {
                floatprint(p);
            } else {
                strprint("Type printing not supported!");
            }
            if constexpr (sizeof...(args)) {
                print_v(str + 2, args...);
            } else {
                if (*(str + 2))
                    strprint(str + 2);
            }
            break;
        }

        putchar(c);
        ++str;
    }
}

/**
 * @brief Function to print to default console, supporting formatting.
 * @remark Thread safety: ST.
 * @tparam Args Type of arguments to be printed if any.
 * @param str String to be printed and/or formatted if needed.
 * @param args Arguments to be formatted into string.
 */
template <typename... Args> void kprint(const char *str, Args... args) {
    if constexpr (sizeof...(args) == 0) {
        strprint(str);
    } else {
        print_v(str, args...);
    }
}

/**
 * @brief Same as kprint, but inserts a new line at the end.
 * @remark Thread safety: ST.
 * @tparam Args Type of arguments to be printed if any.
 * @param str String to be printed and/or formatted if needed.
 * @param args Arguments to be formatted into string.
 */
template <typename... Args> void kprintln(const char *str, Args... args) {
    if constexpr (sizeof...(args) == 0) {
        strprint(str);
    } else {
        print_v(str, args...);
    }

    strprint("\r\n");
}

#ifdef DEBUG
/**
 * @brief Macro to debug expressions/values.
 * @todo Make it so that it is only enabled when some debug macro is defined.
 */
#define kspit(expr)                                                                                                    \
    if constexpr (true) {                                                                                              \
        if constexpr (is_integral_v<decltype(expr)>) {                                                                 \
            const void *p = reinterpret_cast<const void *>(expr);                                                      \
            auto p2 = expr;                                                                                            \
            kprintln(#expr " : {} " #expr " hex: {}", p2, p);                                                          \
        } else {                                                                                                       \
            kprintln(#expr ": {}", expr);                                                                              \
        }                                                                                                              \
    }
#define kdebug(...)                                                                                                    \
    kprint("kdebug: ");                                                                                                \
    kprintln(__VA_ARGS__)
#else
#define kspit(expr)
#define kdebug(expr)
#endif
} // namespace hls

#endif