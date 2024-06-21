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

#ifndef _TYPETRAITS_HPP_
#define _TYPETRAITS_HPP_

#include "misc/types.hpp"

namespace hls
{

    struct true_type
    {
        static constexpr bool value = true;
    };

    struct false_type
    {
        static constexpr bool value = false;
    };

    // is_same trait
    template <typename A, typename B>
    struct is_same : false_type
    {
    };

    template <typename T>
    struct is_same<T, T> : true_type
    {
    };

    template <typename A, typename B>
    constexpr bool is_same_v = is_same<A, B>::value;

    // remove_const trait
    template <typename T>
    struct remove_const
    {
        using type = T;
    };

    template <typename T>
    struct remove_const<const T>
    {
        using type = T;
    };

    template <typename T>
    using remove_const_t = typename remove_const<T>::type;

    // remove_volatile trait
    template <typename T>
    struct remove_volatile
    {
        using type = T;
    };

    template <typename T>
    struct remove_volatile<volatile T>
    {
        using type = T;
    };

    template <typename T>
    using remove_volatile_t = typename remove_volatile<T>::type;

    // remove_reference trait
    template <typename T>
    struct remove_reference
    {
        using type = T;
    };

    template <typename T>
    struct remove_reference<T &>
    {
        using type = T;
    };

    template <typename T>
    struct remove_reference<T &&>
    {
        using type = T;
    };

    template <typename T>
    using remove_reference_t = remove_reference<T>::type;

    // remove_pointer trait
    template <typename T>
    struct remove_pointer
    {
        using type = T;
    };

    template <typename T>
    struct remove_pointer<T *>
    {
        using type = T;
    };

    template <typename T>
    using remove_pointer_t = typename remove_pointer<T>::type;

    // remove_cv trait
    template <typename T>
    struct remove_cv
    {
        using type = remove_const_t<remove_volatile_t<T>>;
    };

    template <typename T>
    using remove_cv_t = remove_cv<T>::type;

    // remove_cvref trait
    template <typename T>
    struct remove_cvref
    {
        using type = remove_cv_t<remove_reference_t<T>>;
    };

    template <typename T>
    using remove_cvref_t = remove_cvref<T>::type;

    // decay trait
    template <typename T>
    struct decay
    {
        using type = remove_cv_t<remove_reference_t<T>>;
    };

    template <typename T, size_t N>
    struct decay<T[N]>
    {
        using type = T *;
    };

    template <typename T, typename... Args>
    struct decay<T(Args...)>
    {
      private:
        T (*funcptr)(Args...);

      public:
        using type = decltype(funcptr);
    };

    template <typename T>
    using decay_t = decay<T>::type;

    // is_reference trait
    template <typename T>
    struct is_reference : false_type
    {
    };

    template <typename T>
    struct is_reference<T &> : true_type
    {
    };

    template <typename T>
    struct is_reference<T &&> : true_type
    {
    };

    template <>
    struct is_reference<void> : false_type
    {
    };

    template <typename T>
    constexpr bool is_reference_v = is_reference<T>::value;

    // is_lvalue_reference trait
    template <typename T>
    struct is_lvalue_reference : false_type
    {
    };

    template <typename T>
    struct is_lvalue_reference<T &> : true_type
    {
    };

    template <typename T>
    constexpr bool is_lvalue_reference_v = is_lvalue_reference<T &>::value;

    template <typename>
    struct is_integral : false_type
    {
    };

    template <>
    struct is_integral<bool> : true_type
    {
    };

    template <>
    struct is_integral<char> : true_type
    {
    };

    template <>
    struct is_integral<unsigned char> : true_type
    {
    };

    template <>
    struct is_integral<short> : true_type
    {
    };

    template <>
    struct is_integral<unsigned short> : true_type
    {
    };

    template <>
    struct is_integral<int> : true_type
    {
    };

    template <>
    struct is_integral<unsigned int> : true_type
    {
    };

    template <>
    struct is_integral<long int> : true_type
    {
    };

    template <>
    struct is_integral<unsigned long int> : true_type
    {
    };

    template <>
    struct is_integral<long long int> : true_type
    {
    };

    template <>
    struct is_integral<unsigned long long int> : true_type
    {
    };

    template <typename T>
    constexpr bool is_integral_v = is_integral<T>::value;

    namespace detail
    {
        template <bool V>
        struct expression_result : true_type
        {
        };
        template <>
        struct expression_result<false> : false_type
        {
        };

        template <typename T>
        struct check_signed : expression_result<T(-1) < T(0)>
        {
        };

        template <typename T>
        struct check_signed<T *>
        {
            static constexpr bool value = false;
        };

    }; // namespace detail

    template <typename T>
    struct is_signed
    {
        static constexpr bool value = is_integral_v<T> && detail::check_signed<T>::value;
    };

    template <typename T>
    constexpr bool is_signed_v = is_signed<T>::value;

    template <typename>
    struct is_pointer : false_type
    {
    };

    template <typename T>
    struct is_pointer<T *> : true_type
    {
    };

    template <typename T>
    struct is_pointer<volatile T *> : true_type
    {
    };

    template <typename T>
    struct is_pointer<const T *> : true_type
    {
    };

    template <typename T>
    struct is_pointer<const volatile T *> : true_type
    {
    };

    template <typename T>
    struct is_pointer<T *const> : true_type
    {
    };

    template <typename T>
    struct is_pointer<volatile T *const> : true_type
    {
    };

    template <typename T>
    struct is_pointer<const T *const> : true_type
    {
    };

    template <typename T>
    struct is_pointer<const volatile T *const> : true_type
    {
    };

    template <typename T>
    constexpr bool is_pointer_v = is_pointer<T>::value;

    template <typename T>
    struct is_floating_point : false_type
    {
    };

    template <>
    struct is_floating_point<float> : true_type
    {
    };

    template <>
    struct is_floating_point<double> : true_type
    {
    };

    template <typename T>
    constexpr bool is_floating_point_v = is_floating_point<T>::value;

    namespace detail
    {
        template <class T>
        struct type_identity
        {
            using type = T;
        };

        template <class T>
        auto try_add_lvalue_reference(int) -> type_identity<T &>;
        template <class T>
        auto try_add_lvalue_reference(...) -> type_identity<T>;

        template <class T>
        auto try_add_rvalue_reference(int) -> type_identity<T &&>;
        template <class T>
        auto try_add_rvalue_reference(...) -> type_identity<T>;
    } // namespace detail

    template <class T>
    struct add_lvalue_reference : decltype(detail::try_add_lvalue_reference<T>(0))
    {
    };

    template <class T>
    struct add_rvalue_reference : decltype(detail::try_add_rvalue_reference<T>(0))
    {
    };

    template <typename T>
    using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;

    template <typename T>
    using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;

    template <typename T, typename, bool>
    class one_of_two
    {
      public:
        using type = T;
    };

    template <typename T, typename U>
    class one_of_two<T, U, true>
    {
      public:
        using type = U;
    };

    template <typename T, typename U, bool b>
    using one_of_two_t = one_of_two<T, U, b>::type;

    template <typename T, typename U>
    class biggest_type
    {
      public:
        using type = one_of_two<T, U, (sizeof(U) > sizeof(T))>::type;
    };

    template <typename T, typename U>
    using biggest_type_t = biggest_type<T, U>::type;

    template <typename T, typename... Args>
    class is_one_of
    {
        template <typename U, typename... args>
        static constexpr bool test_function()
        {
            bool val;

            if constexpr (sizeof...(args) == 0)
                val = hls::is_same_v<T, U>;
            else if constexpr (sizeof...(args) > 0)
                val = test_function<args...>();

            return val || hls::is_same_v<T, U>;
        }

      public:
        static constexpr bool value = test_function<Args...>();
    };

    template <typename T, typename... Args>
    constexpr bool is_one_of_v = is_one_of<T, Args...>::value;

} // namespace hls

#endif
