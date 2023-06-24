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

#include <rtl/limits.hpp>

namespace rtl
{

#ifdef __GNUC__
    // NOTE: if x <= 0, then result is undefined
    [[nodiscard]] constexpr int ceil_log2_i( int x )
    {
        return sizeof( int ) * 8 - __builtin_clz( x - 1 );
    }
#else
    // NOTE: if x <= 0, then result is undefined
    [[nodiscard]] constexpr int ceil_log2_i( int x )
    {
        int result = 0;

        for ( --x; x; x >>= 1 )
            ++result;

        return result;
    }
#endif
    // NOTE: if base == 0 and exponent == 0, then result is undefined
    [[nodiscard]] constexpr int pow_i( int base, int exponent )
    {
        if ( exponent < 0 )
            return abs( base ) == 1 ? base : 0;

        int result = 1;

        for ( ; exponent > 0; --exponent )
        {
            result *= base;
        }

        return result;
    }

} // namespace rtl
