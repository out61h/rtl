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

#include "win.hpp"

namespace rtl
{
    namespace impl
    {
#if RTL_ENABLE_ASSERT || RTL_ENABLE_RUNTIME_CHECKS
        void assert( bool condition, int code, const char* message, const char* file, int line )
        {
            if ( condition )
                return;

            DWORD_PTR args[4];
            args[0] = reinterpret_cast<DWORD_PTR>( message );
            args[1] = reinterpret_cast<DWORD_PTR>( file );
            args[2] = static_cast<DWORD_PTR>( line );
            args[3] = static_cast<DWORD_PTR>( code );

            constexpr int length = 256;
            char          buffer[length];

            ::FormatMessageA( FORMAT_MESSAGE_ARGUMENT_ARRAY | FORMAT_MESSAGE_FROM_STRING,
                              "%4!i!: %1 (%2:%3!i!)",
                              0,
                              0,
                              buffer,
                              length,
                              (va_list*)args );

            ::FatalAppExitA( 0, buffer );
        }
#endif

#if RTL_ENABLE_LOG
        void log( const char* function, const char* fmt... )
        {
            va_list args;
            va_start( args, fmt );

            // NOTE: wsprintfA outputs no more than 1024 symbols per call
            CHAR message[2048];

            CHAR* str = message;

            // TODO: Use safer realization of printf or proove safity (NOTE: StringCbVPrintfA is
            // inapplicable)
            // TODO: %f support
            str += ::wsprintfA( str, "[%s]: ", function );
            str += ::wvsprintfA( str, fmt, args );
            str += ::wsprintfA( str, "\n" );

            va_end( args );

            ::OutputDebugStringA( message );
        }
#endif

#if RTL_ENABLE_APP
    #if RTL_ENABLE_RUNTIME_CHECKS
        namespace win
        {
            void check( bool condition, const char* file, int line )
            {
                if ( condition )
                    return;

                const DWORD code = ::GetLastError();

                constexpr int length = 256;
                char          str[length];

                ::FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM, 0, code, 0, str, length, nullptr );

                assert( false, (int)code, str, file, line );
            }
        } // namespace win
    #endif
#endif

    } // namespace impl
} // namespace rtl
