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

#include <rtl/algorithm.hpp>
#include <rtl/memory.hpp>

namespace rtl
{
    // TODO: implement more methods
    template<typename T>
    class basic_string_view final
    {
    public:
        using value_type = T;

        static constexpr size_t npos = (size_t)-1;

        constexpr basic_string_view()
            : m_data( nullptr )
            , m_size( 0 )
        {
        }

        constexpr basic_string_view( nullptr_t ) = delete;

        // cppcheck-suppress noExplicitConstructor
        constexpr basic_string_view( const value_type* data )
            : m_data( data )
            , m_size( 0 )
        {
            for ( ; data && *data; ++data )
                ++m_size;
        }

        constexpr basic_string_view( const value_type* data, size_t size )
            : m_data( data )
            , m_size( size )
        {
        }

        [[nodiscard]] constexpr const value_type* data() const
        {
            return m_data;
        }

        [[nodiscard]] constexpr size_t size() const
        {
            return m_size;
        }

        [[nodiscard]] constexpr bool empty() const
        {
            return m_size == 0;
        }

        [[nodiscard]] constexpr bool operator==( const basic_string_view<T>& rhs ) const
        {
            if ( size() != rhs.size() )
                return false;

            const value_type* l = data();
            const value_type* r = rhs.data();

            for ( size_t i = 0; i < size(); ++i )
                if ( *l++ != *r++ )
                    return false;

            return true;
        }

        [[nodiscard]] constexpr size_t find( const basic_string_view<T>& what ) const
        {
            // TODO: use Strsafe.h routines?
            const value_type* str = data();
            const value_type* str_end = data() + size();
            const value_type* pattern = what.data();
            const value_type* pattern_end = what.data() + what.size();

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
        const value_type* m_data;
        size_t            m_size;
    };

    // TODO: Implement SSO? (it's not code size friendly)
    // TODO: Align buffer size
    template<typename T>
    class basic_string final
    {
    public:
        using value_type = T;

        static constexpr size_t npos = (size_t)-1;

        constexpr basic_string()
            : m_data( new value_type[1] )
            , m_size( 0 )
        {
            m_data[m_size] = 0;
        }

        constexpr basic_string( size_t size, value_type ch )
            : m_data( new value_type[size + 1] )
            , m_size( size )
        {
            rtl::fill_n( m_data.get(), m_size, ch );
            m_data[size] = 0;
        }

        // cppcheck-suppress noExplicitConstructor
        constexpr basic_string( const value_type* str )
            : basic_string()
        {
            const value_type* last = str;
            for ( ; *last; )
                ++last;

            m_size = static_cast<size_t>( last - str );
            m_data.reset( new value_type[m_size + 1] );
            rtl::copy_n( str, m_size, m_data.get() );
            m_data[m_size] = 0;
        }

        constexpr basic_string( nullptr_t ) = delete;

        constexpr explicit basic_string( const basic_string_view<T>& view )
            // TODO: rtl::make_unique
            : m_data( new value_type[view.size() + 1] )
            , m_size( view.size() )
        {
            // TODO: use Strsafe.h routines?
            value_type*       dst = m_data.get();
            const value_type* src = view.data();

            for ( size_t i = 0; i < view.size(); ++i )
                *dst++ = *src++;

            *dst = 0;
        }

        [[nodiscard]] constexpr const value_type* data() const
        {
            return m_data.get();
        }

        [[nodiscard]] constexpr value_type* data()
        {
            return m_data.get();
        }

        [[nodiscard]] constexpr const value_type* c_str() const
        {
            return m_data.get();
        }

        [[nodiscard]] constexpr size_t size() const
        {
            return m_size;
        }

        [[nodiscard]] constexpr bool empty() const
        {
            return m_size == 0;
        }

        // TODO: basic_string_view::find
        [[nodiscard]] constexpr size_t rfind( const basic_string_view<T>& what ) const
        {
            // TODO: use Strsafe.h routines?
            const value_type* str = data() + size() - 1;
            const value_type* pattern = what.data() + what.size() - 1;

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

        [[nodiscard]] constexpr size_t find( const basic_string_view<T>& what ) const
        {
            return rtl::basic_string_view<value_type>( *this ).find( what );
        }

        [[nodiscard]] constexpr basic_string substr( size_t from, size_t to = npos ) const
        {
            if ( from == npos )
                return basic_string<value_type>();

            return basic_string<value_type>(
                basic_string_view( data() + from, rtl::min( m_size, to ) - from ) );
        }

        [[nodiscard]] constexpr basic_string operator+( basic_string_view<T> rhs ) const
        {
            // TODO: use Strsafe.h routines?
            basic_string<value_type> result( size() + rhs.size(), 0 );

            const value_type* src = data();
            value_type*       dst = result.m_data.get();

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

            const value_type* l = data();
            const value_type* r = rhs.data();

            for ( size_t i = 0; i < size(); ++i )
                if ( *l++ != *r++ )
                    return false;

            return true;
        }

        [[nodiscard]] constexpr bool operator!=( const basic_string_view<T>& rhs ) const
        {
            return !operator==( rhs );
        }

        [[nodiscard]] constexpr operator basic_string_view<T>() const
        {
            return basic_string_view<value_type>( data(), size() );
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
            : m_data( new value_type[other.m_size + 1] )
            , m_size( other.m_size )
        {
            const value_type* src = other.data();
            value_type*       dst = m_data.get();

            for ( size_t i = 0; i < size(); ++i )
                *dst++ = *src++;
        }

        // cppcheck-suppress operatorEq
        constexpr basic_string& operator=( const basic_string& other )
        {
            if ( this != &other )
            {
                m_data.reset( new value_type[other.m_size + 1] );
                m_size = other.m_size;

                const value_type* src = other.data();
                value_type*       dst = m_data.get();

                for ( size_t i = 0; i < size(); ++i )
                    *dst++ = *src++;
            }

            return *this;
        }

    private:
        // TODO: use vector or buffer with size overprovision using capacity?
        rtl::unique_ptr<value_type[]> m_data;
        size_t                        m_size;
    };

    using string = basic_string<char>;
    using string_view = basic_string_view<char>;

    using wstring = basic_string<wchar_t>;
    using wstring_view = basic_string_view<wchar_t>;

    wstring to_wstring( int value );
    wstring to_wstring( unsigned value );
} // namespace rtl
