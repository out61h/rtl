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

// TODO: lightweight release checks with hash code of __FILE__ and __LINE__
// TODO: assert macro with looping until condition becomes true and options like RETRY, ABORT,
// IGNORE

#if RTL_ENABLE_ASSERT
    #define RTL_ASSERT( expr ) rtl::impl::assert( expr, 0u, #expr, __FILE__, __LINE__ )
#else
    #define RTL_ASSERT( expr )
#endif

#if RTL_ENABLE_LOG
    #define RTL_LOG( msg, ... ) rtl::impl::log( __FUNCTION__, msg, __VA_ARGS__ )
#else
    #define RTL_LOG( msg, ... )
#endif

namespace rtl
{
    namespace impl
    {
        void assert( bool condition, int code, const char* message, const char* file, int line );
        void log( const char* function, const char* message... );
    } // namespace impl
} // namespace rtl
