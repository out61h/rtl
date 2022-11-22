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

#if RTL_ENABLE_OPENCL

    #include <rtl/sys/impl/opencl.hpp>
    #include <rtl/sys/opencl.hpp>

namespace rtl
{
    namespace opencl
    {
        static_assert( sizeof( char ) == 1 );
        static_assert( sizeof( rtl::string_view::value_type ) == sizeof( char ) );

        platform_list platform::query_list_by_extension( rtl::string_view extension )
        {
            [[maybe_unused]] cl_int result = 0;

            cl_uint platform_count = 0;
            result = ::clGetPlatformIDs( 0, nullptr, &platform_count );
            RTL_OPENCL_CHECK( result );

            rtl::unique_ptr<cl_platform_id[]> platforms( new cl_platform_id[platform_count] );
            result = ::clGetPlatformIDs( platform_count, platforms.get(), &platform_count );
            RTL_OPENCL_CHECK( result );

            platform_list list;
            list.reserve( platform_count );

            for ( cl_uint i = 0; i < platform_count; ++i )
            {
                size_t ext_bytes_count = 0;

                result = ::clGetPlatformInfo(
                    platforms[i], CL_PLATFORM_EXTENSIONS, 0, nullptr, &ext_bytes_count );
                RTL_OPENCL_CHECK( result );

                rtl::string ext( ext_bytes_count, 0 );

                result = ::clGetPlatformInfo(
                    platforms[i], CL_PLATFORM_EXTENSIONS, ext.size(), ext.data(), nullptr );
                RTL_OPENCL_CHECK( result );

                if ( extension.empty() || ext.find( extension ) != rtl::string::npos )
                    list.push_back( platform( platforms[i] ) );
            }

            return list;
        }

        platform_list platform::query_list()
        {
            return query_list_by_extension( "" );
        }

        platform::platform( void* native_id )
            : m_id( native_id )
        {
        }

        platform::platform( platform&& other )
            : platform()
        {
            *this = rtl::move( other );
        }

        platform& platform::operator=( platform&& other )
        {
            if ( this != &other )
            {
                m_id = nullptr;
                rtl::swap( m_id, other.m_id );
            }

            return *this;
        }
    } // namespace opencl
} // namespace rtl

#endif
