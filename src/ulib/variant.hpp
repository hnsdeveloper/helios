//
//  variant.hpp
//  dummy
//
//  Created by Helio Santos on 21/06/2024.
//

#ifndef _VARIANT_HPP_
#define _VARIANT_HPP_

#include "misc/new.hpp"
#include "misc/utilities.hpp"
#include "sys/mem.hpp"
#include <type_traits>

namespace hls
{

    namespace detail
    {
        template <template <class, class> class Prop, typename T, typename... Args>
        class compare_property
        {
          public:
            using property_first = Prop<T, typename compare_property<Prop, Args...>::property_first>::type;
        };

        template <template <class, class> class Prop, typename T, typename U>
        class compare_property<Prop, T, U>
        {
          public:
            using property_first = typename Prop<T, U>::type;
        };

        template <template <class, class> class Prop, typename T>
        class compare_property<Prop, T>
        {
          public:
            using property_first = T;
        };

        using destructor_fn = void (*)(void *);
        using cpy_constr_fn = void (*)(void *a, const void *b);
        using mv_constr_fn = void (*)(void *a, void *b);

        template <size_t list_size, size_t curr_idx, typename T, typename... Args>
        class Helper
        {
            static_assert(curr_idx < list_size, "Index can't be higher than list size");

            using next = Helper<list_size, curr_idx + 1, Args...>;

            static void destr(void *a)
            {
                T *p = reinterpret_cast<T *>(a);
                p->~T();
            }

            static void cpy(void *a, const void *b)
            {
                new (a) T(*reinterpret_cast<const T *>(b));
            }

            static void move(void *a, void *b)
            {
                new (a) T(hls::move(*reinterpret_cast<T *>(b)));
            }

          public:
            static destructor_fn get_destructor_fn(size_t idx)
            {
                if (idx == curr_idx)
                    return destr;
                return next::get_destructor_fn(idx);
            }

            static cpy_constr_fn get_copy_fn(size_t idx)
            {
                if (idx == curr_idx)
                    return cpy;
                return next::get_copy_fn(idx);
            }

            static mv_constr_fn get_move_fn(size_t idx)
            {
                if (idx == curr_idx)
                    return move;
                return next::get_move_fn(idx);
            }

            static void *aligned_buffer(size_t idx, void *buffer, size_t buffer_size)
            {
                if (idx != curr_idx)
                    return next::aligned_buffer(idx, buffer, buffer_size);

                byte *ptr = reinterpret_cast<byte *>(buffer);
                byte *end = ptr + buffer_size;
                // TODO: Implement
                return (end >= ptr) ? (T *)(ptr) : nullptr;
            }

            template <typename U>
            static constexpr size_t get_type_index()
            {
                if (std::is_same_v<U, T>)
                    return curr_idx;
                return next::template get_type_index<U>();
            }
        };

        template <size_t list_size, size_t curr_idx, typename T>
        class Helper<list_size, curr_idx, T>
        {
            static_assert(curr_idx == list_size - 1, "List size/index mismatch.");

            static void destr(void *a)
            {
                T *p = reinterpret_cast<T *>(a);
                p->~T();
            }

            static void cpy(void *a, const void *b)
            {
                new (a) T(*reinterpret_cast<const T *>(b));
            }

            static void move(void *a, void *b)
            {
                new (a) T(hls::move(*reinterpret_cast<T *>(b)));
            }

          public:
            static destructor_fn get_destructor_fn(size_t)
            {
                return destr;
            }

            static cpy_constr_fn get_copy_fn(size_t)
            {
                return cpy;
            }

            static mv_constr_fn get_move_fn(size_t)
            {
                return move;
            }

            static void *aligned_buffer(size_t idx, void *buffer, size_t buffer_size)
            {
                byte *ptr = reinterpret_cast<byte *>(buffer);
                byte *end = ptr + buffer_size;
                (void)(idx);
                // TODO: Implement
                return (end >= ptr) ? (T *)(ptr) : nullptr;
            }

            template <typename U>
            static constexpr size_t get_type_index()
            {
                return curr_idx;
            }
        };

        template <typename T, typename U>
        class biggest_type
        {
          public:
            using type = std::conditional_t<(sizeof(T) > sizeof(U)), T, U>;
        };

        template <typename T, typename U>
        using biggest_type_t = biggest_type<T, U>::type;

    } // namespace detail

    template <typename T, typename... Args>
    class Variant
    {
        using helper = typename detail::Helper<sizeof...(Args) + 1, 0, T, Args...>;
        using BiggestType = detail::compare_property<detail::biggest_type_t, T, Args...>::property_first;

        alignas(BiggestType) byte m_data[sizeof(BiggestType)];
        size_t m_held_id;
        bool m_has_val;

        void destroy_held()
        {
            if (!is_empty())
            {
                auto dest = helper::get_destructor_fn(get_held_id());
                dest(helper::aligned_buffer(get_held_id(), m_data, sizeof(m_data)));
                m_has_val = false;
            }
            m_held_id = 0;
        }

      public:
        Variant() : m_held_id(0), m_has_val(false) {};

        template <typename U>
            requires(std::is_same_v<U, T> || (std::is_same_v<T, Args> || ...))
        explicit Variant(U &&val)
        {
            using t = std::remove_cvref_t<U>;
            m_held_id = helper::template get_type_index<t>();

            void *ptr = helper::aligned_buffer(get_held_id(), m_data, sizeof(m_data));
            new (ptr) U(hls::move(val));
            m_has_val = true;
        }

        // Uses RVO to construct a variant in place
        template <typename U, typename... UArgs>
            requires(std::is_same_v<U, T> || (std::is_same_v<T, Args> || ...))
        static Variant in_place(UArgs &&...args)
        {
            Variant v;
            size_t type_idx = helper::template get_type_index<U>();
            void *ptr = helper::aligned_buffer(type_idx, v.m_data, sizeof(m_data));
            new (ptr) U(hls::forward<UArgs>(args)...);
            v.m_held_id = type_idx;
            v.m_has_val = true;
            return v;
        }

        Variant(const Variant &other)
        {
            if (!other.is_empty())
            {
                helper::get_copy_fn(other.get_held_id())(this, &other);
                m_held_id = other.get_held_id();
                m_has_val = true;
            }
        }

        Variant(Variant &&other)
        {
            if (!other.is_empty())
            {
                helper::get_move_fn(other.get_held_id())(this, &other);
                m_held_id = other.get_held_id();
                m_has_val = true;
                other.clear();
            }
        }

        ~Variant()
        {
            destroy_held();
        }

        Variant &operator=(const Variant &other)
        {
            if (this != &other)
            {
                destroy_held();
                if (!other.is_empty())
                {
                    helper::get_copy_fn(other.get_held_id())(this, &other);
                    m_has_val = true;
                }
            }
            return *this;
        }

        Variant &operator=(Variant &&other)
        {
            if (this != &other)
            {
                destroy_held();
                if (!other.is_empty())
                {
                    helper::get_move_fn(other.get_held_id())(this, &other);
                    m_held_id = other.get_held_id();
                    m_has_val = true;
                    other.clear();
                }
            }
        }

        template <typename U>
            requires(std::is_same_v<U, T> || (std::is_same_v<T, Args> || ...))
        U &get_value_or_default(U &&def)
        {
            const auto &as_const = *this;
            return const_cast<U &>(as_const.template get_value<U>(hls::forward<U>(def)));
        }

        template <typename U>
            requires(std::is_same_v<U, T> || (std::is_same_v<T, Args> || ...))
        const U &get_value_or_default(U &&def) const
        {
            auto p = get_value_ptr<U>();
            if (p)
                return *p;
            return def;
        }

        template <typename U>
            requires(std::is_same_v<U, T> || (std::is_same_v<T, Args> || ...))
        const U *get_value_ptr() const
        {
            if (!is_empty() && ((get_held_id()) == helper::template get_type_index<U>()))
            {
                auto p = helper::aligned_buffer(get_held_id(), (void *)m_data, sizeof(m_data));
                return reinterpret_cast<const U *>(p);
            }
            return nullptr;
        }

        template <typename U>
            requires(std::is_same_v<U, T> || (std::is_same_v<T, Args> || ...))
        U *get_value_ptr()
        {
            const auto &as_const = *this;
            return const_cast<U *>(as_const.template get_value_ptr<U>());
        }

        void clear()
        {
            destroy_held();
        }

        bool is_empty() const
        {
            return m_has_val != 1;
        }

        template <typename U>
            requires(std::is_same_v<U, T> || (std::is_same_v<T, Args> || ...))
        Variant &operator=(U &&val)
        {
            if (!is_empty())
                clear();
            using type = std::remove_cvref_t<U>;
            size_t type_idx = helper::template get_type_index<type>();
            void *ptr = helper::aligned_buffer(type_idx, m_data, sizeof(m_data));
            new (ptr) type(hls::move(val));
            m_has_val = true;
            return *this;
        }

        size_t get_held_id() const
        {
            return m_held_id;
        }
    };
} // namespace hls

#endif /* variant_hpp */
