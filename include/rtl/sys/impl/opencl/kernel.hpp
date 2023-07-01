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

#if RTL_ENABLE_OPENCL

    #include <rtl/sys/impl/opencl.hpp>
    #include <rtl/sys/opencl.hpp>

namespace rtl
{
    namespace opencl
    {
        kernel::kernel( void* native_object )
            : m_kernel( native_object )
        {
        }

        kernel::kernel( kernel&& other )
            : kernel()
        {
            *this = rtl::move( other );
        }

        kernel& kernel::operator=( kernel&& other )
        {
            if ( this != &other )
            {
                release();

                rtl::swap( m_kernel, other.m_kernel );
            }

            return *this;
        }

        kernel::~kernel()
        {
            release();
        }

        void kernel::release()
        {
            if ( m_kernel )
            {
                [[maybe_unused]] cl_int result
                    = ::clReleaseKernel( static_cast<cl_kernel>( m_kernel ) );
                RTL_OPENCL_CHECK( result );

                m_kernel = nullptr;
            }
        }

        kernel_arg_setter kernel::args()
        {
            return kernel_arg_setter( *this );
        }

        void kernel::set_arg( unsigned index, float value )
        {
            [[maybe_unused]] cl_int result = ::clSetKernelArg(
                static_cast<cl_kernel>( m_kernel ), index, sizeof( value ), &value );
            RTL_OPENCL_CHECK( result );
        }

        void kernel::set_arg( unsigned index, int value )
        {
            const cl_int arg = value;

            [[maybe_unused]] cl_int result = ::clSetKernelArg(
                static_cast<cl_kernel>( m_kernel ), index, sizeof( arg ), &arg );
            RTL_OPENCL_CHECK( result );
        }

        void kernel::set_arg( unsigned index, unsigned int value )
        {
            const cl_uint arg = value;

            [[maybe_unused]] cl_int result = ::clSetKernelArg(
                static_cast<cl_kernel>( m_kernel ), index, sizeof( arg ), &arg );
            RTL_OPENCL_CHECK( result );
        }

        void kernel::set_arg( unsigned index, const buffer& value )
        {
            const cl_mem arg = static_cast<const cl_mem>( value.m_buffer );

            [[maybe_unused]] cl_int result = ::clSetKernelArg(
                static_cast<cl_kernel>( m_kernel ), index, sizeof( arg ), &arg );
            RTL_OPENCL_CHECK( result );
        }

    } // namespace opencl
} // namespace rtl

#endif
