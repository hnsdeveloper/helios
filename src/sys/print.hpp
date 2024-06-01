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

#include "misc/limits.hpp"
#include "misc/types.hpp"
#include "misc/typetraits.hpp"
#include "sys/mem.hpp"
#include "sys/opensbi.hpp"
namespace hls
{

/**
 * @brief Prints string **str** to default console without formatting.
 * @remark Thread safety: ST.
 * @param str String to be printed.
 */
#ifndef __STRPRINT
#define __STRPRINT
#define strprint(__str)                                                                                                \
    [](const char *str) __attribute__((always_inline))                                                                 \
    {                                                                                                                  \
        if (str)                                                                                                       \
        {                                                                                                              \
            while (*str)                                                                                               \
            {                                                                                                          \
                opensbi_putchar(*str);                                                                                 \
                ++str;                                                                                                 \
            }                                                                                                          \
        }                                                                                                              \
    }                                                                                                                  \
    (__str)
#endif

/**
 * @brief Same as strprint, but inserts new line at the end.
 * @remark Thread safety: ST.
 * @param str String to be printed.
 */
#ifndef __STRPRINTLN
#define __STRPRINTLN
#define strprintln(__str2)                                                                                             \
    [](const char *str) __attribute__((always_inline))                                                                 \
    {                                                                                                                  \
        strprint(str);                                                                                                 \
        opensbi_putchar('\r');                                                                                         \
        opensbi_putchar('\n');                                                                                         \
    }                                                                                                                  \
    (__str2)
#endif

/**
 * @brief Prints a pointer value to default console as a hex integral.
 * @remark Thread safety: ST.
 * @param p Pointer value to be printed.
 */
#ifndef __PTRPRINT
#define __PTRPRINT
#define ptrprint(__p)                                                                                                  \
    [](const void *ptr) __attribute__((always_inline))                                                                 \
    {                                                                                                                  \
        auto v = to_uintptr_t(ptr);                                                                                    \
        char buffer[sizeof(v) * 8 / 4];                                                                                \
        for (size_t i = 0; i < sizeof(v) * 8 / 4; ++i)                                                                 \
        {                                                                                                              \
            char c = v & 0xF;                                                                                          \
            if (c <= 9)                                                                                                \
            {                                                                                                          \
                c += '0';                                                                                              \
            }                                                                                                          \
            else                                                                                                       \
            {                                                                                                          \
                c += 'A' - 10;                                                                                         \
            }                                                                                                          \
            buffer[i] = c;                                                                                             \
            v = v >> 4;                                                                                                \
        }                                                                                                              \
        opensbi_putchar('0');                                                                                          \
        opensbi_putchar('x');                                                                                          \
        for (size_t i = 0; i < sizeof(v) * 8 / 4; ++i)                                                                 \
        {                                                                                                              \
            char &c = buffer[sizeof(v) * 8 / 4 - i - 1];                                                               \
            opensbi_putchar(c);                                                                                        \
        }                                                                                                              \
    }                                                                                                                  \
    (__p)
#endif

/**
 * @brief Prints a signed integral value to default console.
 * @remark Thread safety: ST.
 * @param v Value to be printed.
 */
#ifndef __INTPRINT
#define __INTPRINT
#define intprint(__v)                                                                                                  \
    [](int64_t v) __attribute__((always_inline))                                                                       \
    {                                                                                                                  \
        if (v >= 0)                                                                                                    \
            uintprint(0 + v);                                                                                          \
        else                                                                                                           \
        {                                                                                                              \
            opensbi_putchar('-');                                                                                      \
            if (v == hls::limit<int64_t>::min)                                                                         \
            {                                                                                                          \
                v += 1;                                                                                                \
                v = -v;                                                                                                \
                uintprint((uint64_t(0) + v) + 1);                                                                      \
            }                                                                                                          \
            else                                                                                                       \
            {                                                                                                          \
                v = -v;                                                                                                \
                uintprint(v);                                                                                          \
            }                                                                                                          \
        }                                                                                                              \
    }                                                                                                                  \
    (__v)
#endif

/**
 * @brief Prints an unsigned integral value to default console.
 * @remark Thread safety: ST.
 * @param v Value to be printed.
 */
#ifndef __UINTPRINT
#define __UINTPRINT
#define uintprint(__val)                                                                                               \
    [](uint64_t v) __attribute__((always_inline))                                                                      \
    {                                                                                                                  \
        if (v == 0)                                                                                                    \
        {                                                                                                              \
            opensbi_putchar('0');                                                                                      \
            return;                                                                                                    \
        }                                                                                                              \
        char buffer[256];                                                                                              \
        size_t buffer_used = 0;                                                                                        \
        for (size_t i = 0; v; ++i, v /= 10, ++buffer_used)                                                             \
        {                                                                                                              \
            buffer[i] = v % 10 + '0';                                                                                  \
        }                                                                                                              \
        for (size_t i = 0; i < buffer_used; ++i)                                                                       \
        {                                                                                                              \
            char &c = buffer[buffer_used - i - 1];                                                                     \
            opensbi_putchar(c);                                                                                        \
        }                                                                                                              \
    }                                                                                                                  \
    (__val)
#endif

/**
 * @brief Prints a float value to default console.
 * @remark Thread safety: ST.
 * @todo Implement
 * @param d The value to be printed
 */
#ifndef __FLOATPRINT
#define __FLOATPRINT
// TODO: Implement
#define floatprint(__d) [](double d) __attribute__((always_inline)){}(__d)
#endif

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
    template <typename T, typename... Args>
    void print_v(const char *str, T p, Args... args)
    {
        if (str == nullptr)
        {
            return;
        }

        while (*str)
        {
            auto c = *str;
            if (c == '{')
            {
                if (*(str + 1) != '}')
                {
                    strprint("{");
                    print_v(str + 1, p, args...);
                    break;
                }
                using type = decltype(p);
                if constexpr (is_same_v<type, const char *>)
                {
                    strprint(p);
                }
                else if constexpr (is_same_v<remove_cvref_t<type>, char>)
                {
                    opensbi_putchar(p);
                }
                else if constexpr (is_pointer_v<type>)
                {
                    ptrprint(p);
                }
                else if constexpr (is_integral_v<type>)
                {
                    if (is_signed<type>::value)
                    {
                        intprint(p);
                    }
                    else if constexpr (!is_signed<type>::value)
                    {
                        uintprint(p);
                    }
                }
                else if constexpr (is_floating_point_v<type>)
                {
                    floatprint(p);
                }
                else
                {
                    strprint("Type printing not supported!");
                }
                if constexpr (sizeof...(args))
                {
                    print_v(str + 2, args...);
                }
                else
                {
                    if (*(str + 2))
                        strprint(str + 2);
                }
                break;
            }

            opensbi_putchar(c);
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
    template <typename... Args>
    void kprint(const char *str, Args... args)
    {
        if constexpr (sizeof...(args) == 0)
        {
            strprint(str);
        }
        else
        {
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
    template <typename... Args>
    void kprintln(const char *str, Args... args)
    {
        if constexpr (sizeof...(args) == 0)
        {
            strprint(str);
        }
        else
        {
            print_v(str, args...);
        }

        strprintln(nullptr);
    }

#ifdef DEBUG
/**
 * @brief Macro to debug expressions/values.
 * @todo Make it so that it is only enabled when some debug macro is defined.
 */
#define kspit(expr)                                                                                                    \
    if constexpr (true)                                                                                                \
    {                                                                                                                  \
        if constexpr (is_integral_v<decltype(expr)>)                                                                   \
        {                                                                                                              \
            const void *p = reinterpret_cast<const void *>(expr);                                                      \
            auto p2 = expr;                                                                                            \
            kprintln(#expr " : {} " #expr " hex: {}", p2, p);                                                          \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
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