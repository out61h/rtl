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
    #include <rtl/sys/debug.hpp>
    #include <rtl/sys/impl/opencl.hpp>
    #include <rtl/sys/opencl.hpp>

namespace rtl
{
    namespace opencl
    {
        device::device( void* id )
            : m_id( id )
        {
            cl_device_id device_id = static_cast<cl_device_id>( id );

            [[maybe_unused]] cl_int result;

            {
                size_t name_byte_count = 0;
                result
                    = ::clGetDeviceInfo( device_id, CL_DEVICE_NAME, 0, nullptr, &name_byte_count );
                RTL_OPENCL_CHECK( result );

                m_name = string( name_byte_count - 1, 0 );

                result = ::clGetDeviceInfo(
                    device_id, CL_DEVICE_NAME, name_byte_count, m_name.data(), nullptr );
                RTL_OPENCL_CHECK( result );
            }
            {
                size_t version_byte_count = 0;

                result = ::clGetDeviceInfo(
                    device_id, CL_DEVICE_VERSION, 0, nullptr, &version_byte_count );
                RTL_OPENCL_CHECK( result );

                m_version = string( version_byte_count - 1, 0 );

                result = ::clGetDeviceInfo(
                    device_id, CL_DEVICE_VERSION, version_byte_count, m_version.data(), nullptr );
                RTL_OPENCL_CHECK( result );
            }
            {
                size_t extensions_byte_count = 0;

                result = ::clGetDeviceInfo(
                    device_id, CL_DEVICE_EXTENSIONS, 0, nullptr, &extensions_byte_count );
                RTL_OPENCL_CHECK( result );

                m_extensions = string( extensions_byte_count - 1, 0 );

                result = ::clGetDeviceInfo( device_id,
                                            CL_DEVICE_EXTENSIONS,
                                            extensions_byte_count,
                                            m_extensions.data(),
                                            nullptr );
                RTL_OPENCL_CHECK( result );
            }
            {
                size_t vendor_byte_count = 0;

                result = ::clGetDeviceInfo(
                    device_id, CL_DEVICE_VENDOR, 0, nullptr, &vendor_byte_count );
                RTL_OPENCL_CHECK( result );

                m_vendor = string( vendor_byte_count, 0 );

                result = ::clGetDeviceInfo(
                    device_id, CL_DEVICE_VENDOR, m_vendor.size(), m_vendor.data(), nullptr );
                RTL_OPENCL_CHECK( result );
            }
        }

        device::device( device&& other )
            : device()
        {
            *this = rtl::move( other );
        }

        device& device::operator=( device&& other )
        {
            if ( this != &other )
            {
                m_id = nullptr;
                rtl::swap( m_id, other.m_id );
                m_name = rtl::move( other.m_name );
                m_version = rtl::move( other.m_version );
                m_vendor = rtl::move( other.m_vendor );
                m_extensions = rtl::move( other.m_extensions );
            }

            return *this;
        }

        device_list device::query_list( const platform_list& platforms )
        {
            device_list devices;

            cl_int result;

            for ( const auto& platform : platforms )
            {
                cl_uint device_count = 0;

                result = ::clGetDeviceIDs( static_cast<cl_platform_id>( platform.m_id ),
                                           CL_DEVICE_TYPE_ALL,
                                           0,
                                           nullptr,
                                           &device_count );
                RTL_OPENCL_CHECK( result );

                rtl::vector<cl_device_id> device_ids( device_count );

                result = ::clGetDeviceIDs( static_cast<cl_platform_id>( platform.m_id ),
                                           CL_DEVICE_TYPE_ALL,
                                           device_ids.size(),
                                           device_ids.data(),
                                           nullptr );
                RTL_OPENCL_CHECK( result );

                for ( const auto& device_id : device_ids )
                {
                    cl_bool avaliable;

                    result = ::clGetDeviceInfo(
                        device_id, CL_DEVICE_AVAILABLE, sizeof( avaliable ), &avaliable, nullptr );
                    RTL_OPENCL_CHECK( result );

                    if ( !avaliable )
                        continue;

                    result = ::clGetDeviceInfo( device_id,
                                                CL_DEVICE_COMPILER_AVAILABLE,
                                                sizeof( avaliable ),
                                                &avaliable,
                                                nullptr );
                    RTL_OPENCL_CHECK( result );

                    if ( !avaliable )
                        continue;

                    // TODO: emplace_back
                    devices.push_back( device( device_id ) );
                }
            }

            return devices;
        }

        [[nodiscard]] bool device::operator==( const device& rhs ) const
        {
            return m_id == rhs.m_id;
        }

        [[nodiscard]] bool device::operator!=( const device& rhs ) const
        {
            return !operator==( rhs );
        }

        [[nodiscard]] bool device::extension_supported( rtl::string_view extension ) const
        {
            return m_extensions.find( extension ) != rtl::string::npos;
        }

    } // namespace opencl
} // namespace rtl

#endif
