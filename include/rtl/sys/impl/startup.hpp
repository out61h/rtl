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

#include "heap.hpp"
#include "tests.hpp"
#include "win.hpp"

extern "C"
{
    int _fltused = 0;
}

#pragma comment( linker, "/ENTRY:rtl_entry_point" )

extern void main( void );

extern "C" void __stdcall rtl_entry_point( void )
{
#if RTL_ENABLE_HEAP
    rtl::impl::g_heap.init();
#endif

#if RTL_ENABLE_RUNTIME_TESTS
    rtl::impl::runtime_tests::run();
#endif

    main();
    ::ExitProcess( 0 );
}
