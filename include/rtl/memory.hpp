/*
 * Copyright (C) 2016-2022 Konstantin Polevik
 * All rights reserved
 *
 * This file is part of the RTL library. Redistribution and use in source and
 * binary forms, with or without modification, are permitted exclusively
 * under the terms of the MIT license. You should have received a copy of the
 * license with this file. If not, please visit:
 * https://github.com/out61h/rtl/blob/main/LICENSE.
 */
#pragma once

#include <rtl/int.hpp>
#include <rtl/move.hpp>

namespace rtl
{
    using nullptr_t = decltype( nullptr );

    namespace impl
    {
        template<typename T>
        struct default_deleter
        {
            void operator()( T* p )
            {
                static_assert( sizeof( T ) > 0, "default_deleter cannot delete incomplete type" );
                delete p;
            }

            // TODO: rtl::pair (packed)
            int pad;
        };

        template<typename T>
        struct default_deleter<T[]>
        {
            void operator()( T* p )
            {
                static_assert( sizeof( T ) > 0, "default_deleter cannot delete incomplete type" );
                delete[] p;
            }

            // TODO: rtl::pair (packed)
            int pad;
        };

        template<typename T>
        struct pointer
        {
            using type = T*;
        };

        template<typename T>
        struct pointer<T[]>
        {
            using type = T*;
        };

    } // namespace impl

    template<typename T, typename D = impl::default_deleter<T>>
    class unique_ptr final
    {
    public:
        using element_type = T;
        using deleter_type = D;
        using pointer = typename impl::pointer<T>::type;

        constexpr unique_ptr()
            : m_ptr( pointer() )
        {
        }

        constexpr explicit unique_ptr( pointer p )
            : m_ptr( p )
        {
        }

        constexpr explicit unique_ptr( pointer p, deleter_type deleter )
            : m_ptr( p )
            , m_deleter( deleter )
        {
        }

        ~unique_ptr()
        {
            reset();
        }

        constexpr unique_ptr( unique_ptr&& other )
            : m_ptr( other.release() )
            , m_deleter( other.m_deleter )
        {
        }

        constexpr unique_ptr& operator=( unique_ptr&& other )
        {
            if ( this != &other )
            {
                reset( other.release() );
                m_deleter = other.m_deleter;
            }

            return *this;
        }

        [[nodiscard]] constexpr pointer operator->()
        {
            return m_ptr;
        }

        [[nodiscard]] constexpr const pointer operator->() const
        {
            return m_ptr;
        }

        [[nodiscard]] constexpr explicit operator bool() const
        {
            return m_ptr != nullptr;
        }

        [[nodiscard]] constexpr const pointer get() const
        {
            return m_ptr;
        }

        [[nodiscard]] constexpr pointer get()
        {
            return m_ptr;
        }

        constexpr void reset( pointer p = pointer() )
        {
            pointer old_ptr = m_ptr;
            m_ptr = p;

            if ( old_ptr )
                m_deleter( old_ptr );
        }

        [[nodiscard]] constexpr pointer release()
        {
            pointer result = m_ptr;
            m_ptr = pointer();
            return result;
        }

    private:
        unique_ptr( const unique_ptr& ) = delete;
        unique_ptr& operator=( const unique_ptr& ) = delete;

        pointer      m_ptr;
        deleter_type m_deleter;
    };

    // TODO: specialization for [] with operator[]
    // TODO: make_unique
} // namespace rtl
