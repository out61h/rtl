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
        context::context( context&& other )
            : context()
        {
            *this = rtl::move( other );
        }

        context& context::operator=( context&& other )
        {
            if ( this != &other )
            {
                release();

                rtl::swap( m_device, other.m_device );
                rtl::swap( m_context, other.m_context );
                rtl::swap( m_command_queue, other.m_command_queue );
            }

            return *this;
        }

        void context::release()
        {
            if ( m_command_queue )
            {
                [[maybe_unused]] cl_int result
                    = ::clReleaseCommandQueue( static_cast<cl_command_queue>( m_command_queue ) );
                RTL_OPENCL_CHECK( result );

                m_command_queue = nullptr;
            }

            if ( m_context )
            {
                [[maybe_unused]] cl_int result
                    = ::clReleaseContext( static_cast<cl_context>( m_context ) );
                RTL_OPENCL_CHECK( result );

                m_context = nullptr;
            }
        }

        context context::create_with_current_ogl_context( const device& device )
        {
            cl_int status;

            cl_device_id device_id = static_cast<cl_device_id>( device.m_id );

            cl_platform_id platform;
            status = ::clGetDeviceInfo(
                device_id, CL_DEVICE_PLATFORM, sizeof( platform ), &platform, 0 );
            RTL_OPENCL_CHECK( status );

            cl_context_properties context_props[]{
                CL_GL_CONTEXT_KHR,
                ( cl_context_properties )::wglGetCurrentContext(),
                CL_WGL_HDC_KHR,
                ( cl_context_properties )::wglGetCurrentDC(),
                CL_CONTEXT_PLATFORM,
                (cl_context_properties)platform,
                0 };

            cl_context context_object
                = ::clCreateContext( context_props, 1, &device_id, nullptr, nullptr, &status );
            RTL_OPENCL_CHECK( status );

            cl_queue_properties queue_props[]{ 0 };

            cl_command_queue queue_object = ::clCreateCommandQueueWithProperties(
                context_object, device_id, queue_props, &status );
            RTL_OPENCL_CHECK( status );

            return context( device_id, context_object, queue_object );
        }

        context::context( void* device, void* context, void* command_queue )
            : m_device( device )
            , m_context( context )
            , m_command_queue( command_queue )
        {
        }

        context::~context()
        {
            release();
        }

        program context::build_program( rtl::string_view source )
        {
            const char*  sources[]{ source.data() };
            const size_t sources_length[]{ source.size() };

            cl_int status;

            cl_program program_object = ::clCreateProgramWithSource(
                static_cast<cl_context>( m_context ), 1, sources, sources_length, &status );
            RTL_OPENCL_CHECK( status );

            cl_device_id device_id = static_cast<cl_device_id>( m_device );
            status = ::clBuildProgram( program_object, 0, nullptr, "-Werror", nullptr, nullptr );

            if ( status != CL_SUCCESS )
            {
                cl_build_status build_status;
                status = ::clGetProgramBuildInfo( program_object,
                                                  device_id,
                                                  CL_PROGRAM_BUILD_STATUS,
                                                  sizeof( cl_build_status ),
                                                  &build_status,
                                                  nullptr );
                RTL_OPENCL_CHECK( status );

                if ( build_status != CL_BUILD_SUCCESS )
                {
                    size_t log_size;

                    status = ::clGetProgramBuildInfo(
                        program_object, device_id, CL_PROGRAM_BUILD_LOG, 0, nullptr, &log_size );
                    RTL_OPENCL_CHECK( status );

                    rtl::string log( log_size, 0 );
                    status = ::clGetProgramBuildInfo( program_object,
                                                      device_id,
                                                      CL_PROGRAM_BUILD_LOG,
                                                      log_size,
                                                      log.data(),
                                                      nullptr );
                    RTL_OPENCL_CHECK( status );

                    RTL_LOG( "OpenCL program build log:\r\n%s", log.c_str() );

                    return program();
                }
            }

            return program( program_object );
        }

        void context::enqueue_acquire_ogl_object( buffer& buffer )
        {
            cl_mem mem = static_cast<cl_mem>( buffer.m_buffer );

            [[maybe_unused]] cl_int result = ::clEnqueueAcquireGLObjects(
                static_cast<cl_command_queue>( m_command_queue ), 1, &mem, 0, nullptr, nullptr );

            RTL_OPENCL_CHECK( result );
        }

        void context::enqueue_release_ogl_object( buffer& buffer )
        {
            cl_mem mem = static_cast<cl_mem>( buffer.m_buffer );

            [[maybe_unused]] cl_int result = ::clEnqueueReleaseGLObjects(
                static_cast<cl_command_queue>( m_command_queue ), 1, &mem, 0, nullptr, nullptr );
            RTL_OPENCL_CHECK( result );
        }

        void context::enqueue_process_1d( const kernel& kernel, size_t dim1 )
        {
            [[maybe_unused]] cl_int result
                = ::clEnqueueNDRangeKernel( static_cast<cl_command_queue>( m_command_queue ),
                                            static_cast<cl_kernel>( kernel.m_kernel ),
                                            1,
                                            nullptr,
                                            &dim1,
                                            nullptr,
                                            0,
                                            nullptr,
                                            nullptr );
            RTL_OPENCL_CHECK( result );
        }

        void context::enqueue_process_2d( const kernel& kernel, size_t dim1, size_t dim2 )
        {
            const size_t image_size[2]{ dim1, dim2 };

            [[maybe_unused]] cl_int result
                = ::clEnqueueNDRangeKernel( static_cast<cl_command_queue>( m_command_queue ),
                                            static_cast<cl_kernel>( kernel.m_kernel ),
                                            2,
                                            nullptr,
                                            image_size,
                                            nullptr,
                                            0,
                                            nullptr,
                                            nullptr );
            RTL_OPENCL_CHECK( result );
        }

        void context::enqueue_copy( const buffer& src, float* dst, size_t size )
        {
            [[maybe_unused]] cl_int result
                = ::clEnqueueReadBuffer( static_cast<cl_command_queue>( m_command_queue ),
                                         static_cast<cl_mem>( src.m_buffer ),
                                         CL_FALSE,
                                         0,
                                         sizeof( cl_float ) * size,
                                         dst,
                                         0,
                                         nullptr,
                                         nullptr );
            RTL_OPENCL_CHECK( result );
        }

        void context::enqueue_copy( const buffer& src, uint32_t* dst, size_t size )
        {
            [[maybe_unused]] cl_int result
                = ::clEnqueueReadBuffer( static_cast<cl_command_queue>( m_command_queue ),
                                         static_cast<cl_mem>( src.m_buffer ),
                                         CL_FALSE,
                                         0,
                                         sizeof( cl_uint ) * size,
                                         dst,
                                         0,
                                         nullptr,
                                         nullptr );
            RTL_OPENCL_CHECK( result );
        }

        void context::enqueue_copy( const uint32_t* src, buffer& dst, size_t size )
        {
            [[maybe_unused]] cl_int result
                = ::clEnqueueWriteBuffer( static_cast<cl_command_queue>( m_command_queue ),
                                          static_cast<cl_mem>( dst.m_buffer ),
                                          CL_FALSE,
                                          0,
                                          sizeof( cl_uint ) * size,
                                          src,
                                          0,
                                          nullptr,
                                          nullptr );
            RTL_OPENCL_CHECK( result );
        }

        void context::enqueue_copy( const bool* src, buffer& dst, size_t size )
        {
            [[maybe_unused]] cl_int result
                = ::clEnqueueWriteBuffer( static_cast<cl_command_queue>( m_command_queue ),
                                          static_cast<cl_mem>( dst.m_buffer ),
                                          CL_FALSE,
                                          0,
                                          sizeof( cl_bool ) * size,
                                          src,
                                          0,
                                          nullptr,
                                          nullptr );
            RTL_OPENCL_CHECK( result );
        }

        void context::enqueue_copy( const buffer& src, buffer& dst )
        {
            RTL_ASSERT( src.length() == dst.length() );
            RTL_ASSERT( src.element_size() == dst.element_size() );
            RTL_ASSERT( src.length() != 0 );
            RTL_ASSERT( src.element_size() != 0 );

            [[maybe_unused]] cl_int result
                = ::clEnqueueCopyBuffer( static_cast<cl_command_queue>( m_command_queue ),
                                         static_cast<cl_mem>( src.m_buffer ),
                                         static_cast<cl_mem>( dst.m_buffer ),
                                         0,
                                         0,
                                         src.element_size() * src.length(),
                                         0,
                                         nullptr,
                                         nullptr );
            RTL_OPENCL_CHECK( result );
        }

        void context::wait()
        {
            [[maybe_unused]] cl_int status
                = ::clFinish( static_cast<cl_command_queue>( m_command_queue ) );
            RTL_OPENCL_CHECK( status );
        }

        buffer context::create_buffer_1d_float( size_t size ) const
        {
            RTL_ASSERT( size > 0 );

            cl_int status;
            cl_mem mem_object = ::clCreateBuffer( static_cast<cl_context>( m_context ),
                                                  CL_MEM_READ_WRITE,
                                                  sizeof( cl_float ) * size,
                                                  nullptr,
                                                  &status );
            RTL_OPENCL_CHECK( status );

            return buffer( mem_object, sizeof( cl_float ), size );
        }

        buffer context::create_buffer_1d_float( size_t size, const cl_float* data ) const
        {
            RTL_ASSERT( size > 0 );
            RTL_ASSERT( data != nullptr );

            cl_int status;
            cl_mem mem_object = ::clCreateBuffer(
                static_cast<cl_context>( m_context ),
                CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                sizeof( cl_float ) * size,
                const_cast<cl_float*>( data ), // OpenCL should not write to buffer (I hope :)
                &status );
            RTL_OPENCL_CHECK( status );

            return buffer( mem_object, sizeof( cl_float ), size );
        }

        buffer context::create_buffer_1d_byte( size_t size, const cl_uchar* data ) const
        {
            RTL_ASSERT( size > 0 );
            RTL_ASSERT( data != nullptr );

            cl_int status;
            cl_mem mem_object = ::clCreateBuffer(
                static_cast<cl_context>( m_context ),
                CL_MEM_COPY_HOST_PTR | CL_MEM_READ_WRITE,
                sizeof( cl_uchar ) * size,
                const_cast<cl_uchar*>( data ), // OpenCL should not write to buffer (I hope :)
                &status );
            RTL_OPENCL_CHECK( status );

            return buffer( mem_object, sizeof( cl_uchar ), size );
        }

        buffer context::create_buffer_1d_byte( size_t size ) const
        {
            RTL_ASSERT( size > 0 );

            cl_int status;
            cl_mem mem_object = ::clCreateBuffer( static_cast<cl_context>( m_context ),
                                                  CL_MEM_READ_WRITE,
                                                  sizeof( cl_uchar ) * size,
                                                  nullptr,
                                                  &status );
            RTL_OPENCL_CHECK( status );

            return buffer( mem_object, sizeof( cl_uchar ), size );
        }

        buffer context::create_buffer_1d_uint( size_t size ) const
        {
            RTL_ASSERT( size > 0 );

            cl_int status;
            cl_mem mem_object = ::clCreateBuffer( static_cast<cl_context>( m_context ),
                                                  CL_MEM_READ_WRITE,
                                                  sizeof( cl_uint ) * size,
                                                  nullptr,
                                                  &status );
            RTL_OPENCL_CHECK( status );

            return buffer( mem_object, sizeof( cl_uint ), size );
        }

        buffer context::create_buffer_1d_uint( size_t size, const cl_uint* data ) const
        {
            RTL_ASSERT( size > 0 );
            RTL_ASSERT( data != nullptr );

            cl_int status;
            cl_mem mem_object = ::clCreateBuffer(
                static_cast<cl_context>( m_context ),
                CL_MEM_COPY_HOST_PTR | CL_MEM_READ_WRITE,
                sizeof( cl_uint ) * size,
                const_cast<cl_uint*>( data ), // OpenCL should not write to buffer (I hope :)
                &status );
            RTL_OPENCL_CHECK( status );

            return buffer( mem_object, sizeof( cl_uint ), size );
        }

        buffer context::create_buffer_2d_from_ogl_texture( uint32_t texture_id ) const
        {
            cl_int status;
            cl_mem mem_object = ::clCreateFromGLTexture( static_cast<cl_context>( m_context ),
                                                         CL_MEM_WRITE_ONLY,
                                                         GL_TEXTURE_2D,
                                                         0,
                                                         texture_id,
                                                         &status );
            RTL_OPENCL_CHECK( status );

            return buffer( mem_object, 1, 1 );
        }
    } // namespace opencl

} // namespace rtl

#endif
