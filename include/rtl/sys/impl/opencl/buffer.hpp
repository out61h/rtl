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
        buffer::buffer( void* buffer, rtl::size_t element_size, rtl::size_t length )
            : m_buffer( buffer )
            , m_length( length )
            , m_element_size( element_size )
        {
        }

        buffer::buffer( buffer&& other )
            : buffer()
        {
            *this = rtl::move( other );
        }

        buffer& buffer::operator=( buffer&& other )
        {
            if ( this != &other )
            {
                release();

                rtl::swap( m_buffer, other.m_buffer );
                rtl::swap( m_length, other.m_length );
                rtl::swap( m_element_size, other.m_element_size );
            }

            return *this;
        }

        buffer::~buffer()
        {
            release();
        }

        void buffer::release()
        {
            if ( m_buffer )
            {
                [[maybe_unused]] cl_int result
                    = ::clReleaseMemObject( static_cast<cl_mem>( m_buffer ) );
                RTL_OPENCL_CHECK( result );

                m_buffer = nullptr;
                m_length = 0;
                m_element_size = 0;
            }
        }
    } // namespace opencl
} // namespace rtl

#endif
