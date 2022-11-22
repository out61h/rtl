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
        program::program( void* native_object )
            : m_program( native_object )
        {
        }

        program::program( program&& other )
            : program()
        {
            *this = rtl::move( other );
        }

        program& program::operator=( program&& other )
        {
            if ( this != &other )
            {
                release();

                rtl::swap( m_program, other.m_program );
            }

            return *this;
        }

        void program::release()
        {
            if ( m_program )
            {
                [[maybe_unused]] cl_int result
                    = ::clReleaseProgram( static_cast<cl_program>( m_program ) );
                RTL_OPENCL_CHECK( result );

                m_program = nullptr;
            }
        }

        kernel program::create_kernel( const rtl::string& kernel_name ) const
        {
            cl_int status;

            cl_kernel native_object = ::clCreateKernel(
                static_cast<cl_program>( m_program ), kernel_name.c_str(), &status );
            RTL_OPENCL_CHECK( status );

            return kernel( native_object );
        }

        program::~program()
        {
            release();
        }
    } // namespace opencl
} // namespace rtl

#endif
