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

#include <rtl/memory.hpp>

namespace rtl
{
    // TODO: implement more methods
    template<typename T>
    class basic_string_view final
    {
    public:
        static constexpr size_t npos = (size_t)-1;

        constexpr basic_string_view()
            : m_data( nullptr )
            , m_size( 0 )
        {
        }

        constexpr basic_string_view( nullptr_t ) = delete;

        // cppcheck-suppress noExplicitConstructor
        constexpr basic_string_view( const T* data )
            : m_data( data )
            , m_size( 0 )
        {
            for ( ; data && *data; ++data )
                ++m_size;
        }

        constexpr basic_string_view( const T* data, size_t size )
            : m_data( data )
            , m_size( size )
        {
        }

        [[nodiscard]] constexpr const T* data() const
        {
            return m_data;
        }

        [[nodiscard]] constexpr size_t size() const
        {
            return m_size;
        }

        [[nodiscard]] constexpr bool operator==( const basic_string_view<T>& rhs ) const
        {
            if ( size() != rhs.size() )
                return false;

            const T* l = data();
            const T* r = rhs.data();

            for ( size_t i = 0; i < size(); ++i )
                if ( *l++ != *r++ )
                    return false;

            return true;
        }

        // TODO: implement also in basic_string
        [[nodiscard]] constexpr size_t find( const basic_string_view<T>& what ) const
        {
            // TODO: use Strsafe.h routines?
            const T* str = data();
            const T* str_end = data() + size();
            const T* pattern = what.data();
            const T* pattern_end = what.data() + what.size();

            for ( ; str < str_end; ++str )
            {
                if ( *str == *pattern )
                {
                    if ( ++pattern >= pattern_end )
                        return (size_t)( str - data() );
                }
                else
                {
                    pattern = what.data();
                }
            }

            return npos;
        }

    private:
        const T* m_data;
        size_t   m_size;
    };

    // TODO: Implement SSO? (it's not code size friendly)
    // TODO: Align buffer size
    template<typename T>
    class basic_string final
    {
    public:
        static constexpr size_t npos = (size_t)-1;

        constexpr basic_string()
            : m_size( 0 )
        {
        }

        constexpr basic_string( nullptr_t ) = delete;

        constexpr explicit basic_string( const basic_string_view<T>& view )
            // TODO: rtl::make_unique
            : m_data( new T[view.size() + 1] )
            , m_size( view.size() )
        {
            // TODO: use Strsafe.h routines?
            T*       dst = m_data.get();
            const T* src = view.data();

            for ( size_t i = 0; i < view.size(); ++i )
                *dst++ = *src++;

            *dst = 0;
        }

        [[nodiscard]] constexpr const T* data() const
        {
            return m_data.get();
        }

        [[nodiscard]] constexpr const T* c_str() const
        {
            return m_data.get();
        }

        [[nodiscard]] constexpr size_t size() const
        {
            return m_size;
        }

        [[nodiscard]] constexpr size_t rfind( const basic_string_view<T>& what ) const
        {
            // TODO: use Strsafe.h routines?
            const T* str = data() + size() - 1;
            const T* pattern = what.data() + what.size() - 1;

            for ( ; str >= data(); --str )
            {
                if ( *str == *pattern )
                {
                    if ( --pattern < what.data() )
                        return (size_t)( str - data() );
                }
                else
                {
                    pattern = what.data() + what.size() - 1;
                }
            }

            return npos;
        }

        [[nodiscard]] constexpr basic_string substr( size_t from, size_t to = npos ) const
        {
            if ( from == npos )
                return basic_string<T>();

            return basic_string<T>(
                basic_string_view( data() + from, rtl::min( m_size, to ) - from ) );
        }

        [[nodiscard]] constexpr basic_string operator+( const basic_string_view<T>& rhs ) const
        {
            // TODO: use Strsafe.h routines?
            basic_string<T> result;
            result.m_size = size() + rhs.size();
            result.m_data.reset( new T[result.m_size + 1] );

            const wchar_t* src = data();
            wchar_t*       dst = result.m_data.get();

            for ( size_t i = 0; i < size(); ++i )
                *dst++ = *src++;

            src = rhs.data();
            dst = result.m_data.get() + size();

            for ( size_t i = 0; i < rhs.size(); ++i )
                *dst++ = *src++;

            *dst = 0;

            return result;
        }

        [[nodiscard]] constexpr bool operator==( const basic_string_view<T>& rhs ) const
        {
            // TODO: use Strsafe.h routines?
            if ( size() != rhs.size() )
                return false;

            const T* l = data();
            const T* r = rhs.data();

            for ( size_t i = 0; i < size(); ++i )
                if ( *l++ != *r++ )
                    return false;

            return true;
        }

        [[nodiscard]] constexpr bool operator!=( const basic_string_view<T>& rhs ) const
        {
            return !operator==( rhs );
        }

        constexpr operator basic_string_view<T>() const
        {
            return basic_string_view<T>( data(), size() );
        }

        constexpr basic_string( basic_string&& other )
            : basic_string()
        {
            *this = rtl::move( other );
        }

        constexpr basic_string& operator=( basic_string&& other )
        {
            if ( this != &other )
            {
                m_data.reset( other.m_data.release() );
                m_size = other.m_size;
                other.m_size = 0;
            }

            return *this;
        }

        constexpr basic_string( const basic_string& other )
            : m_data( new T[other.m_size + 1] )
            , m_size( other.m_size )
        {
            const T* src = other.data();
            T*       dst = m_data.get();

            for ( size_t i = 0; i < size(); ++i )
                *dst++ = *src++;
        }

        // cppcheck-suppress operatorEq
        constexpr basic_string& operator=( const basic_string& other )
        {
            if ( this != &other )
            {
                m_data.reset( new T[other.m_size + 1] );
                m_size = other.m_size;

                const T* src = other.data();
                T*       dst = m_data.get();

                for ( size_t i = 0; i < size(); ++i )
                    *dst++ = *src++;
            }

            return *this;
        }

    private:
        rtl::unique_ptr<T[]> m_data;
        size_t               m_size;
    };

    using string = basic_string<char>;
    using string_view = basic_string_view<char>;

    using wstring = basic_string<wchar_t>;
    using wstring_view = basic_string_view<wchar_t>;
} // namespace rtl
