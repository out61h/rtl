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

#include "debug.hpp"
#include "win.hpp"

// TODO: move declaration that defined in headers of `sys` folder to namespace rtl::sys?
namespace rtl
{
    int wsprintf_s( wchar_t* buffer, size_t /* buffer_size */, const wchar_t* fmt... )
    {
        va_list args;
        va_start( args, fmt );

        // TODO: Use safer realization of printf or proove safity (NOTE: StringCbVPrintfW is
        // inapplicable)
        // TODO: %f support
        const int result = wvsprintfW( buffer, fmt, args );

        va_end( args );

        return result;
    }

    int wsprintf_s( wchar_t* buffer, size_t /*buffer_size*/, const char* fmt... )
    {
        constexpr size_t fmt_buffer_size = 1024;
        wchar_t          fmt_buffer[fmt_buffer_size];

        [[maybe_unused]] const int res = ::MultiByteToWideChar(
            CP_UTF8, MB_PRECOMPOSED, fmt, -1, fmt_buffer, fmt_buffer_size );
        RTL_WINAPI_CHECK( res != 0 );

        va_list args;
        va_start( args, fmt );

        // TODO: Use safer realization of printf or proove safity (NOTE: StringCbVPrintfW is
        // inapplicable)
        // TODO: %f support
        const int result = ::wvsprintfW( buffer, fmt_buffer, args );

        va_end( args );

        return result;
    }
} // namespace rtl
