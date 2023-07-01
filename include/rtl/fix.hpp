/*
 * Copyright (C) 2016-2023 Konstantin Polevik
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
#include <rtl/limits.hpp>

namespace rtl
{
    template<typename Int, int FractBits>
    class fix final
    {
    public:
        using type = fix<Int, FractBits>;
        using value_type = Int;

        static constexpr int total_bits = sizeof( value_type ) * 8;
        static constexpr int fract_bits = FractBits;
        static constexpr int int_bits = total_bits - fract_bits;

        static_assert( int_bits + fract_bits == total_bits );
        static_assert( numeric_limits<value_type>::is_signed );

        fix() = default;

        explicit constexpr fix( int v )
            : value( v << fract_bits )
        {
            // TODO: check range
        }

        explicit constexpr fix( float f )
            : value( static_cast<value_type>( f * (float)( 1 << fract_bits ) ) )
        {
            // TODO: check range
        }

        [[nodiscard]] static constexpr type from_fraction( int numerator, int denominator )
        {
            // TODO: check range
            return type::from_value( ( numerator << fract_bits ) / denominator );
        }

        [[nodiscard]] static constexpr type from_exp2i( int i )
        {
            // TODO: check range
            return type::from_value( i > 0 ? ( ( 1 << fract_bits ) << i )
                                           : ( ( 1 << fract_bits ) >> i ) );
        }

        [[nodiscard]] static constexpr type min()
        {
            return type::from_value( 1 );
        }

        [[nodiscard]] static constexpr type max()
        {
            return type::from_value( rtl::numeric_limits<Int>::max() );
        }

        [[nodiscard]] constexpr type operator+( const type& rhs ) const
        {
            return type::from_value( value + rhs.value );
        }

        [[nodiscard]] constexpr type operator-( const type& rhs ) const
        {
            return type::from_value( value - rhs.value );
        }

        [[nodiscard]] constexpr type operator-( float rhs ) const
        {
            return operator-( type( rhs ) );
        }

        [[nodiscard]] constexpr type operator*( const type& rhs ) const
        {
            // TODO: check overflow
            return type::from_value( ( value * rhs.value ) >> fract_bits );
        }

        [[nodiscard]] constexpr type operator*( int rhs ) const
        {
            // TODO: check overflow
            return type::from_value( value * rhs );
        }

        [[nodiscard]] constexpr type operator*( float rhs ) const
        {
            return operator*( type( rhs ) );
        }

        [[nodiscard]] constexpr type operator/( const type& rhs ) const
        {
            // TODO: check overflow
            return type::from_value( ( value << fract_bits ) / rhs.value );
        }

        [[nodiscard]] constexpr type operator/( int div ) const
        {
            return type::from_value( value / div );
        }

        [[nodiscard]] constexpr type operator%( int div ) const
        {
            const value_type divisor = div << fract_bits;
            return type::from_value( value - ( value / divisor ) * divisor );
        }

        [[nodiscard]] constexpr type operator-() const
        {
            return type::from_value( -value );
        }

        constexpr type& operator+=( const type& rhs )
        {
            value += rhs.value;
            return *this;
        }

        constexpr type& operator-=( const type& rhs )
        {
            value -= rhs.value;
            return *this;
        }

        constexpr type& operator*=( int rhs )
        {
            // TODO: check overflow
            value *= rhs;
            return *this;
        }

        constexpr type& operator/=( int rhs )
        {
            value /= rhs;
            return *this;
        }

        [[nodiscard]] constexpr bool operator==( const type& rhs ) const
        {
            return value == rhs.value;
        }

        [[nodiscard]] constexpr bool operator!=( const type& rhs ) const
        {
            return value != rhs.value;
        }

        [[nodiscard]] constexpr bool operator!=( int rhs ) const
        {
            return value != ( rhs << fract_bits );
        }

        [[nodiscard]] constexpr bool operator<( const type& rhs ) const
        {
            return value < rhs.value;
        }

        [[nodiscard]] constexpr bool operator<( int rhs ) const
        {
            return value < ( rhs << fract_bits );
        }

        [[nodiscard]] constexpr bool operator<=( const type& rhs ) const
        {
            return value <= rhs.value;
        }

        [[nodiscard]] constexpr bool operator>=( const type& rhs ) const
        {
            return value >= rhs.value;
        }

        [[nodiscard]] constexpr bool operator>=( int rhs ) const
        {
            return value >= ( rhs << fract_bits );
        }

        [[nodiscard]] constexpr bool operator>( const type& rhs ) const
        {
            return value > rhs.value;
        }

        [[nodiscard]] constexpr bool operator>( int rhs ) const
        {
            return value > ( rhs << fract_bits );
        }

        [[nodiscard]] constexpr explicit operator int() const
        {
            return value >> fract_bits;
        }

        [[nodiscard]] constexpr explicit operator float() const
        {
            return (float)value / (float)( 1 << fract_bits );
        }

    private:
        value_type value;

        static constexpr type from_value( value_type value )
        {
            type type;
            type.value = value;
            return type;
        }
    };
} // namespace rtl
