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

#define ANSI_ONLY
#define OEMRESOURCE
#define STRICT
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#define NO
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCOMM
#define NOCTLMGR
#define NODEFERWINDOWPOS
#define NOGDICAPMASKS
#define NOHELP
#define NOICONS
#define NOIMAGE
#define NOIME
#define NOKANJI
#define NOKEYSTATES
#define NOMB
#define NOMCX
#define NOMEMMGR
#define NOMENUS
#define NOMETAFILE
#define NOMINMAX
#define NOOPENFILE
#define NOPROFILER
#define NOPROXYSTUB
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOSYSCOMMANDS
#define NOSYSMETRICS
#define NOTAPE
#define NOWH
#define NOWINRES

#include <Windows.h>

#if RTL_ENABLE_WINAPI_DIAGNOSTICS
    #define RTL_WINAPI_CHECK( condition ) rtl::impl::win::check( condition, __FILE__, __LINE__ )
#else
    #define RTL_WINAPI_CHECK( condition )
#endif

namespace rtl
{
    namespace impl
    {
        namespace win
        {
            void check( bool condition, const char* file, int line );
        }
    } // namespace impl
} // namespace rtl
