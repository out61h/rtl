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

#include <rtl/sys/debug.hpp>
#include <rtl/sys/filesystem.hpp>

#include "win.hpp"

namespace rtl
{
    namespace filesystem
    {
        directory_entry::directory_entry( const filesystem::path& p )
            : m_path( p )
        {
            refresh();
        }

        bool directory_entry::is_regular_file() const
        {
            return m_attributes
                   & ( FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_READONLY
                       | FILE_ATTRIBUTE_COMPRESSED );
        }

        bool directory_entry::is_directory() const
        {
            return m_attributes & FILE_ATTRIBUTE_DIRECTORY;
        }

        void directory_entry::refresh()
        {
            WIN32_FILE_ATTRIBUTE_DATA data;

            m_attributes = 0;
            m_file_size = 0;

            const BOOL result
                = ::GetFileAttributesExW( m_path.c_str(), GetFileExInfoStandard, &data );

            if ( result )
            {
                m_file_size = ( ( static_cast<uintmax_t>( data.nFileSizeHigh ) << 32ull ) )
                              | data.nFileSizeLow;

                m_attributes = data.dwFileAttributes;
            }
        }

        bool directory_entry::operator==( const directory_entry& rhs ) const
        {
            return m_path == rhs.m_path;
        }

        directory_iterator::directory_iterator()
            : m_handle( INVALID_HANDLE_VALUE )
        {
        }

        directory_iterator::directory_iterator( const filesystem::path& path )
        {
            WIN32_FIND_DATAW data;

            m_handle = ::FindFirstFileExW( ( path.wstring() + L"/*" ).c_str(),
                                           FindExInfoBasic,
                                           &data,
                                           FindExSearchNameMatch,
                                           nullptr,
                                           0 );

            if ( m_handle != INVALID_HANDLE_VALUE )
                m_entry = directory_entry( filesystem::path( data.cFileName ) );
        }

        directory_iterator::~directory_iterator()
        {
            if ( m_handle != INVALID_HANDLE_VALUE )
            {
                [[maybe_unused]] BOOL result = ::FindClose( m_handle );
                RTL_WINAPI_CHECK( result );
            }
        }

        directory_iterator& directory_iterator::operator++()
        {
            WIN32_FIND_DATAW data;

            BOOL result = ::FindNextFileW( m_handle, &data );

            if ( !result )
            {
                RTL_WINAPI_CHECK( ::GetLastError() == ERROR_NO_MORE_FILES );

                result = ::FindClose( m_handle );
                RTL_WINAPI_CHECK( result );

                m_handle = INVALID_HANDLE_VALUE;

                m_entry = directory_entry();
            }
            else
            {
                m_entry = directory_entry( path( data.cFileName ) );
            }

            return *this;
        }

        const directory_entry& directory_iterator::operator*() const
        {
            return m_entry;
        }

        bool directory_iterator::operator==( const directory_iterator& rhs )
        {
            return ( m_handle == rhs.m_handle )
                   && ( m_handle == INVALID_HANDLE_VALUE || m_entry == rhs.m_entry );
        }

        bool directory_iterator::operator!=( const directory_iterator& rhs )
        {
            return !operator==( rhs );
        }

        directory_iterator::directory_iterator( directory_iterator&& other )
            : directory_iterator()
        {
            *this = rtl::move( other );
        }

        directory_iterator& directory_iterator::operator=( directory_iterator&& other )
        {
            if ( this != &other )
            {
                m_handle = rtl::move( other.m_handle );
                m_entry = rtl::move( other.m_entry );

                other.m_handle = INVALID_HANDLE_VALUE;
            }

            return *this;
        }

        size_t read_file_content( const wchar_t* name, void* buffer, size_t buffer_size )
        {
            HANDLE file = ::CreateFileW(
                name, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );

            if ( file == INVALID_HANDLE_VALUE )
                return 0;

            const DWORD bytes_to_read = buffer_size;
            DWORD       bytes_read = 0;

            BOOL result = ::ReadFile( file, buffer, bytes_to_read, &bytes_read, FALSE );
            RTL_WINAPI_CHECK( result );

            result = ::CloseHandle( file );
            RTL_WINAPI_CHECK( result );

            return bytes_read;
        }

        file::file()
            : m_handle( INVALID_HANDLE_VALUE )
        {
        }

        file file::open( const wchar_t* name, access acc, mode md )
        {
            DWORD cf_access{ 0 };

            switch ( acc )
            {
            case access::read_only:
                cf_access = GENERIC_READ;
                break;

            case access::write_only:
                cf_access = GENERIC_WRITE;
                break;

            case access::read_write:
                cf_access = GENERIC_READ | GENERIC_WRITE;
                break;
            }

            DWORD cf_mode{ 0 };

            switch ( md )
            {
            case mode::create_always:
                cf_mode = CREATE_ALWAYS;
                break;

            case mode::create_new:
                cf_mode = CREATE_NEW;
                break;

            case mode::open_always:
                cf_mode = OPEN_ALWAYS;
                break;

            case mode::open_existing:
                cf_mode = OPEN_EXISTING;
                break;

            case mode::truncate_existing:
                cf_mode = TRUNCATE_EXISTING;
                break;
            }

            HANDLE handle = ::CreateFileW(
                name, cf_access, FILE_SHARE_READ, nullptr, cf_mode, FILE_ATTRIBUTE_NORMAL, 0 );

            return file( handle );
        }

        file::~file()
        {
            close();
        }

        file::file( file&& other )
            : file()
        {
            *this = rtl::move( other );
        }

        file& file::operator=( file&& other )
        {
            if ( this != &other )
            {
                close();
                rtl::swap( m_handle, other.m_handle );
            }

            return *this;
        }

        void file::close()
        {
            if ( m_handle != INVALID_HANDLE_VALUE )
            {
                [[maybe_unused]] BOOL result = ::CloseHandle( m_handle );
                RTL_WINAPI_CHECK( result );

                m_handle = INVALID_HANDLE_VALUE;
            }
        }

        file::operator bool() const
        {
            return m_handle != INVALID_HANDLE_VALUE;
        }

        intmax_t file::tell()
        {
            LARGE_INTEGER position{ 0 };

            [[maybe_unused]] BOOL result
                = ::SetFilePointerEx( m_handle, { 0 }, &position, FILE_CURRENT );
            RTL_WINAPI_CHECK( result );

            return position.QuadPart;
        }

        void file::seek( intmax_t offs, position pos )
        {
            LARGE_INTEGER offset;
            offset.QuadPart = offs;

            DWORD method{ 0 };

            switch ( pos )
            {
            case position::current:
                method = FILE_CURRENT;
                break;

            case position::begin:
                method = FILE_BEGIN;
                break;

            case position::end:
                method = FILE_END;
                break;
            }

            [[maybe_unused]] BOOL result = ::SetFilePointerEx( m_handle, offset, nullptr, method );
            RTL_WINAPI_CHECK( result );
        }

        unsigned file::read( void* buffer, unsigned size )
        {
            const DWORD bytes_to_read = size;
            DWORD       bytes_read;

            [[maybe_unused]] BOOL result
                = ::ReadFile( m_handle, buffer, bytes_to_read, &bytes_read, nullptr );
            RTL_WINAPI_CHECK( result );

            return bytes_read;
        }

        unsigned file::write( const void* buffer, unsigned size )
        {
            const DWORD bytes_to_write = size;
            DWORD       bytes_written;

            [[maybe_unused]] BOOL result
                = ::WriteFile( m_handle, buffer, bytes_to_write, &bytes_written, nullptr );
            RTL_WINAPI_CHECK( result );

            return bytes_written;
        }

        file::file( void* handle )
            : m_handle( handle )
        {
        }
    } // namespace filesystem
} // namespace rtl
