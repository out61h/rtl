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

#if RTL_ENABLE_OPENCL

    #include <rtl/memory.hpp>
    #include <rtl/string.hpp>
    #include <rtl/vector.hpp>

namespace rtl
{
    namespace opencl
    {
        class platform;
        using platform_list = rtl::vector<platform>;

        class platform final
        {
        public:
            static platform_list query_list_by_extension( rtl::string_view extension );
            static platform_list query_list();

            platform() = default;
            platform( platform&& );
            platform& operator=( platform&& );

        private:
            friend class device;

            explicit platform( void* native_id );
            platform( const platform& ) = delete;
            platform& operator=( const platform& ) = delete;

            void* m_id{ nullptr };
        };

        class device;
        using device_list = rtl::vector<device>;

        class device final
        {
        public:
            static device_list query_list( const platform_list& platforms );

            device() = default;
            device( device&& );
            device& operator=( device&& );

            [[nodiscard]] const rtl::string& name() const
            {
                return m_name;
            }

            [[nodiscard]] const rtl::string& version() const
            {
                return m_version;
            }

        private:
            friend class context;

            explicit device( void* native_id );
            device( const device& ) = delete;
            device& operator=( const device& ) = delete;

            void*       m_id{ nullptr };
            rtl::string m_name;
            rtl::string m_version;
        };

        class buffer final
        {
        public:
            buffer() = default;
            buffer( buffer&& );
            buffer& operator=( buffer&& );
            ~buffer();

            size_t length() const
            {
                return m_length;
            }

            size_t element_size() const
            {
                return m_element_size;
            }

        private:
            friend class kernel;
            friend class context;

            buffer( void* buffer, size_t element_size, size_t length );
            buffer( const buffer& ) = delete;
            buffer& operator=( const buffer& ) = delete;

            void release();

            void*  m_buffer{ nullptr };
            size_t m_length{ 0 };
            size_t m_element_size{ 0 };
        };

        class kernel_arg_setter;

        class kernel final
        {
        public:
            kernel() = default;
            kernel( kernel&& );
            kernel& operator=( kernel&& );
            ~kernel();

            void set_arg( unsigned index, float value );
            void set_arg( unsigned index, const buffer& value );
            void set_arg( unsigned index, int value );
            void set_arg( unsigned index, unsigned int value );

            kernel_arg_setter args();

        private:
            friend class program;
            friend class context;

            explicit kernel( void* native_object );

            kernel( const kernel& ) = delete;
            kernel& operator=( const kernel& ) = delete;

            void release();

            void* m_kernel{ nullptr };
        };

        class kernel_arg_setter final
        {
        public:
            explicit kernel_arg_setter( kernel& kernel )
                : kernel( &kernel )
            {
            }

            kernel_arg_setter( kernel_arg_setter&& that )
            {
                *this = rtl::move( that );
            }

            kernel_arg_setter& operator=( kernel_arg_setter&& that )
            {
                if ( this != &that )
                {
                    kernel = that.kernel;
                    counter = that.counter;
                    that.kernel = nullptr;
                    that.counter = 0;
                }

                return *this;
            }

            template<typename T>
            kernel_arg_setter& arg( const T& value )
            {
                if ( kernel )
                    kernel->set_arg( counter++, value );

                return *this;
            }

        private:
            kernel_arg_setter( const kernel_arg_setter& ) = delete;
            kernel_arg_setter& operator=( const kernel_arg_setter& ) = delete;

            kernel*  kernel{ nullptr };
            unsigned counter{ 0 };
        };

        class program final
        {
        public:
            program() = default;
            program( program&& );
            program& operator=( program&& );
            ~program();

            kernel create_kernel( const rtl::string& kernel_name ) const;

        private:
            friend class context;

            explicit program( void* native_object );
            program( const program& ) = delete;
            program& operator=( const program& ) = delete;

            void release();

            void* m_program{ nullptr };
        };

        class context final
        {
        public:
            context() = default;
            ~context();
            context( context&& );
            context& operator=( context&& );

            static context create_with_current_ogl_context( const device& device );

            program build_program( rtl::string_view source );

            void enqueue_acquire_ogl_object( buffer& buffer );
            void enqueue_release_ogl_object( buffer& buffer );
            void enqueue_process_1d( const kernel& kernel, size_t dim1 );
            void enqueue_process_2d( const kernel& kernel, size_t dim1, size_t dim2 );
            void enqueue_copy( const buffer& src, float* dst, size_t dim );
            void enqueue_copy( const buffer& src, uint32_t* dst, size_t dim );
            void enqueue_copy( const buffer& src, buffer& dst );
            void enqueue_copy( const uint32_t* src, buffer& dst, size_t dim );
            void enqueue_copy( const bool* src, buffer& dst, size_t dim );
            void wait();

            buffer create_buffer_1d_float( size_t size ) const;
            buffer create_buffer_1d_float( size_t size, const float* array_data ) const;
            buffer create_buffer_2d_from_ogl_texture( uint32_t texture_id ) const;
            buffer create_buffer_1d_byte( size_t size, const uint8_t* array_data ) const;
            buffer create_buffer_1d_byte( size_t size ) const;
            buffer create_buffer_1d_uint( size_t size, const uint32_t* array_data ) const;
            buffer create_buffer_1d_uint( size_t size ) const;

        private:
            context( void* device, void* context, void* command_queue );
            context( const context& ) = delete;
            context& operator=( const context& ) = delete;

            void release();

            void* m_device{ nullptr };
            void* m_context{ nullptr };
            void* m_command_queue{ nullptr };
        };
    } // namespace opencl
} // namespace rtl

#endif
