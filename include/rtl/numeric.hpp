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
#include <rtl/type_traits.hpp>

namespace rtl
{
    template<typename M, typename N>
    constexpr common_type_t<M, N> gcd( M m, N n )
    {
        common_type_t<M, N> m_v = abs( m );
        common_type_t<M, N> n_v = abs( n );

        for ( ; n_v; )
        {
            m_v %= n_v;
            rtl::swap( m_v, n_v );
        }

        return m_v;
    }

    template<typename M, typename N>
    constexpr common_type_t<M, N> lcm( M m, N n )
    {
        common_type_t<M, N> m_v = abs( m );
        common_type_t<M, N> n_v = abs( n );

        return m_v / gcd( m_v, n_v ) * n_v;
    }

    template<typename T>
    constexpr int sign( T val )
    {
        return ( T( 0 ) < val ) - ( val < T( 0 ) );
    }
} // namespace rtl
