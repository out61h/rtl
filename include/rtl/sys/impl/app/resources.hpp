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

#include <rtl/sys/impl/application.hpp>

#if RTL_ENABLE_APP
    #if RTL_ENABLE_APP_RESOURCES

namespace rtl
{
    application::resource::operator bool() const
    {
        return m_data != nullptr;
    }

    size_t application::resource::size() const
    {
        return m_size;
    }

    const void* application::resource::data() const
    {
        return m_data;
    }

    application::resource::resource( void* data, size_t size )
        : m_data( data )
        , m_size( size )
    {
    }

    application::resource application::environment::resources::open( int type, int id ) const
    {
        // NOTE: Accordingly to WinAPI docs we don't need to close resource handles explicitly
        HRSRC hrc = ::FindResourceW( nullptr, MAKEINTRESOURCE( id ), MAKEINTRESOURCE( type ) );
        RTL_WINAPI_CHECK( hrc != nullptr );

        HGLOBAL hglobal = ::LoadResource( nullptr, hrc );
        RTL_WINAPI_CHECK( hglobal != nullptr );

        DWORD size = ::SizeofResource( nullptr, hrc );
        RTL_WINAPI_CHECK( size != 0 );

        LPVOID pointer = ::LockResource( hglobal );
        RTL_WINAPI_CHECK( pointer != 0 );

        return application::resource( pointer, size );
    }
} // namespace rtl
    #endif
#endif
