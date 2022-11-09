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

#include <rtl/move.hpp>

namespace rtl
{
    template<typename T>
    constexpr void swap( T& t1, T& t2 )
    {
        T temp = rtl::move( t1 );
        t1 = rtl::move( t2 );
        t2 = rtl::move( temp );
    }

    template<typename T>
    [[nodiscard]] constexpr T clamp( T val, T min, T max )
    {
        return val > max ? max : val < min ? min : val;
    }

    template<typename T>
    [[nodiscard]] constexpr T min( T val1, T val2 )
    {
        return val1 > val2 ? val2 : val1;
    }

    template<typename T>
    [[nodiscard]] constexpr T max( T val1, T val2 )
    {
        return val1 < val2 ? val2 : val1;
    }

    template<typename T>
    [[nodiscard]] constexpr T abs( T val )
    {
        return val < T( 0 ) ? -val : val;
    }

    template<typename InputIterator, typename Size, typename OutputIterator>
    constexpr OutputIterator copy_n( InputIterator src, Size count, OutputIterator dst )
    {
        for ( auto last = dst + count; dst != last; )
            *dst++ = *src++;

        return dst;
    }

    template<typename Iterator, typename Size, typename Value>
    constexpr Iterator fill_n( Iterator first, Size count, const Value& value )
    {
        for ( auto last = first + count; first != last; ++first )
            *first = value;

        return first;
    }

    template<typename Iterator, typename Value>
    constexpr void fill( Iterator first, Iterator last, const Value& value )
    {
        for ( ; first != last; ++first )
            *first = value;
    }

    template<typename InputIterator, typename OutputIterator, typename Operation>
    constexpr OutputIterator transform( InputIterator  first,
                                        InputIterator  last,
                                        OutputIterator output,
                                        Operation      op )
    {
        for ( ; first != last; ++first, ++output )
            *output = op( *first );

        return output;
    }

    template<typename Iterator, typename Operation>
    constexpr void transform( Iterator first, Iterator last, Operation op )
    {
        for ( ; first != last; ++first )
            *first = op( *first );
    }
} // namespace rtl
