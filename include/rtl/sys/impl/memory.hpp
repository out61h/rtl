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

#ifndef RTL_IMPLEMENTATION
    #error "Do not include implementation header directly, use <rtl/sys/impl.hpp>"
#endif

#include <rtl/int.hpp>

#include "heap.hpp"

#if RTL_ENABLE_MEMSET

    #pragma function( memset )

extern "C" void* __cdecl memset( void* dest, int ch, size_t count );

void* __cdecl memset( void* dest, int ch, size_t count )
{
    char* dst = static_cast<char*>( dest );

    for ( ; count; --count )
        *dst++ = static_cast<char>( ch );

    return dest;
}

#endif

#if RTL_ENABLE_HEAP

namespace rtl
{
    namespace impl
    {
        heap g_heap;
    }
} // namespace rtl

// TODO: Implement placement new

[[nodiscard]] void* operator new( size_t count )
{
    return rtl::impl::g_heap.calloc( 1, count );
}

void operator delete( void* p )
{
    rtl::impl::g_heap.free( p );
}

void operator delete( void* p, size_t )
{
    rtl::impl::g_heap.free( p );
}

[[nodiscard]] void* operator new[]( size_t count )
{
    return rtl::impl::g_heap.calloc( count, 1 );
}

void operator delete[]( void* p ) noexcept
{
    rtl::impl::g_heap.free( p );
}

void operator delete[]( void* p, size_t ) noexcept
{
    rtl::impl::g_heap.free( p );
}

#endif
