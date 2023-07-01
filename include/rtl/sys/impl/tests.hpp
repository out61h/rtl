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

#include <rtl/math.hpp>
#include <rtl/string.hpp>

#include <rtl/sys/debug.hpp>
#include <rtl/sys/filesystem.hpp>

#if RTL_ENABLE_RUNTIME_TESTS
    #define RTL_TEST( expr ) rtl::impl::assert( expr, 0, #expr, __FILE__, __LINE__ )
#else
    #define RTL_TEST( expr )
#endif

namespace rtl
{
    namespace impl
    {
        namespace static_tests
        {
            namespace math
            {
                static_assert( pow_i( 10, 2 ) == 100 );
                static_assert( pow_i( 0, 2 ) == 0 );
                static_assert( pow_i( 10, 0 ) == 1 );
                static_assert( pow_i( -1, 0 ) == 1 );
                static_assert( pow_i( -1, 1 ) == -1 );
                static_assert( pow_i( -2, 2 ) == 4 );
                static_assert( pow_i( 1, -2 ) == 1 );
                static_assert( pow_i( 1, -1 ) == 1 );
                static_assert( pow_i( -1, -1 ) == -1 );
                static_assert( pow_i( -1, 1 ) == -1 );
                static_assert( pow_i( 2, -2 ) == 0 );
            } // namespace math

        } // namespace static_tests

#if RTL_ENABLE_RUNTIME_TESTS
        namespace runtime_tests
        {
            namespace string
            {
                void run()
                {
                    rtl::string_view sv( "name.ext" );
                    RTL_TEST( sv == "name.ext" );
                    RTL_TEST( sv.size() == 8 );

                    rtl::string s( "name.ext" );
                    RTL_TEST( s == "name.ext" );
                    RTL_TEST( s.size() == 8 );
                    RTL_TEST( s.rfind( ".ext" ) == 4 );

                    rtl::string_view sext = s.substr( 4, rtl::string::npos );
                    RTL_TEST( sext.size() == 4 );
                    RTL_TEST( sext == ".ext" );
                }
            } // namespace string

            namespace filesystem
            {
                void run()
                {
                    rtl::filesystem::directory_entry de;
                    RTL_TEST( de.exists() == false );
                    RTL_TEST( de.is_directory() == false );
                    RTL_TEST( de.is_regular_file() == false );
                    RTL_TEST( de.file_size() == 0 );

                    rtl::filesystem::path p( L"name.ext" );
                    RTL_TEST( p.extension().wstring() == L".ext" );
                }
            } // namespace filesystem

            void run()
            {
                string::run();
                filesystem::run();
            }
        } // namespace runtime_tests
#endif

    } // namespace impl
} // namespace rtl
