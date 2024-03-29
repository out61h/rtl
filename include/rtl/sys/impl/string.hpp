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

#ifndef RTL_IMPLEMENTATION
    #error "Do not include implementation header directly, use <rtl/sys/impl.hpp>"
#endif

#include <rtl/string.hpp>
#include <rtl/sys/printf.hpp>

namespace rtl
{
    wstring to_wstring( int value )
    {
        wchar_t buffer[16];
        wsprintf_s( buffer, "%i", value );

        return wstring( buffer );
    }

    wstring to_wstring( unsigned value )
    {
        wchar_t buffer[16];
        wsprintf_s( buffer, "%u", value );

        return wstring( buffer );
    }

    wstring to_wstring( const rtl::string& string )
    {
        rtl::vector<wchar_t> buffer( string.size() + 1 );

        // TODO: Remove %S hack and use explicit conversion
        wsprintf_s( buffer.data(), buffer.size(), "%S", string.c_str() );

        return wstring( buffer.data() );
    }
} // namespace rtl
