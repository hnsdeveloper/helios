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

#ifndef _EXPECTED_HPP_
#define _EXPECTED_HPP_

#include "misc/macros.hpp"
#include "misc/new.hpp"
#include "misc/types.hpp"
#include "misc/utilities.hpp"
#include "sys/panic.hpp"
#include <type_traits>

namespace hls
{

    enum class Error : uint64_t
    {
        UNDEFINED_ERROR,
        NOT_FOUND,
        INVALID_ARGUMENT,
        INVALID_INDEX,
        INVALID_PAGE_TABLE,
        INVALID_PAGE_ENTRY,
        INVALID_VIRTUAL_ADDRESS,
        MISALIGNED_MEMORY_ADDRESS,
        ADDRESS_ALREADY_MAPPED,
        NOT_ENOUGH_CONTIGUOUS_MEMORY,
        OUT_OF_MEMORY,
        OUT_OF_BOUNDS,
        VALUE_LIMIT_REACHED,
        READ_NOT_ALLOWED,
        WRITE_NOT_ALLOWED,
        EXECUTE_NOT_ALLOWED,
        OPERATION_NOT_ALLOWED,
        OVERFLOW,
        CORRUPTED_DATA_STRUCTURE
    };

    template <typename T>
        requires(!std::is_reference_v<T>)
    class Result
    {

        SET_USING_CLASS(T, value_type);
        SET_USING_CLASS(Error, error_type);

        union {
            Error e;
            alignas(T) char value[sizeof(T)];
        } m_stored;

        bool m_is_error = false;

        Result(value_type_const_reference v)
        {
            new (reinterpret_cast<value_type *>(m_stored.value)) T(v);
        }

        Result(value_type_rvalue_reference v)
        {
            using type = std::remove_cvref_t<T>;
            new (reinterpret_cast<value_type *>(m_stored.value)) type(hls::move(v));
        }

        Result(error_type e)
        {
            m_stored.e = e;
            m_is_error = true;
        }

      public:
        Result(Result &&other)
        {
            if (other.is_error())
            {
                m_stored.e = other.m_stored.e;
            }
            else
            {
                auto *p = reinterpret_cast<value_type *>(m_stored.value);
                new (p) T(hls::move(reinterpret_cast<value_type &>(other.m_stored.value)));
            }
        }

        Result(const Result &other)
        {
            if (other.is_error())
            {
                m_stored.e = other.m_stored.e;
            }
            else
            {
                auto *p = reinterpret_cast<value_type_ptr>(m_stored.value);
                new (p) T(reinterpret_cast<value_type_const_reference>(other.m_stored.value));
            }
        }

        ~Result()
        {
            if (is_value())
            {
                reinterpret_cast<value_type_ptr>(m_stored.value)->~T();
            }
        }

        bool is_error() const
        {
            return m_is_error;
        }

        bool is_value() const
        {
            return !is_error();
        }

        value_type_reference get_value()
        {
            const auto &a = *this;
            return const_cast<value_type_reference>(a.get_value());
        }

        // Given that a value is expected, in Kernel code if it doesn't hold, we
        // panic, after all, the system might end in a unusable state if we consume
        // the value as if it was proper.
        value_type_const_reference get_value() const
        {
            if (is_error())
            {
                PANIC("Result contains error instead of expected value.");
            }
            return *reinterpret_cast<const value_type *>(m_stored.value);
        }

        error_type get_error() const
        {
            if (!is_error())
            {
                PANIC("Result contains value instead of error.");
            }
            return m_stored.e;
        }

        static auto error(error_type e)
        {
            using type = std::remove_cvref_t<value_type>;
            return Result<type>(e);
        }

        static auto value(value_type_rvalue_reference v)
        {
            using type = std::remove_cvref_t<value_type>;

            return Result<type>(hls::move(v));
        }

        static Result value(value_type_const_reference v)
        {
            using type = std::remove_cvref_t<value_type>;
            return Result<type>(v);
        }
    };

    template <typename T>
    auto error = &Result<T>::error;

    auto value(auto v)
    {
        using type = std::remove_cvref_t<decltype(v)>;
        return Result<type>::value(hls::forward<decltype(v)>(v));
    }

} // namespace hls

#endif
