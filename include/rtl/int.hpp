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

namespace rtl
{
    static_assert( sizeof( int ) == 4 );
    static_assert( sizeof( long long ) == 8 );

    typedef long long          intmax_t;
    typedef unsigned long long uintmax_t;
    typedef signed int         ptrdiff_t;
    typedef unsigned int       size_t;
    typedef signed long long   int64_t;
    typedef unsigned long long uint64_t;
    typedef signed int         int32_t;
    typedef unsigned int       uint32_t;
    typedef unsigned short     uint16_t;
    typedef signed short       int16_t;
    typedef unsigned char      uint8_t;
    typedef char               int8_t;

    static_assert( sizeof( unsigned ) == 4 );
    static_assert( sizeof( unsigned long ) == 4 );

    static_assert( sizeof( uintmax_t ) == 8 );
    static_assert( sizeof( uint64_t ) == 8 );
    static_assert( sizeof( uint32_t ) == 4 );
    static_assert( sizeof( uint16_t ) == 2 );
    static_assert( sizeof( uint8_t ) == 1 );
    static_assert( sizeof( size_t ) == sizeof( ptrdiff_t ) );
} // namespace rtl
