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
    #if RTL_ENABLE_APP_SCREEN_BUFFER

        #if RTL_ENABLE_APP_OPENGL
            #error "RTL_ENABLE_APP_OPENGL and RTL_ENABLE_APP_SCREEN_BUFFER are mutually exclusive"
        #endif

namespace rtl
{
    namespace impl
    {
        namespace win
        {
            void window::init_screen_buffer( int width, int height )
            {
                HDC hdc = ::GetDC( m_window_handle );
                RTL_WINAPI_CHECK( hdc != nullptr );

                m_screen_buffer_dc = ::CreateCompatibleDC( hdc );
                RTL_WINAPI_CHECK( m_screen_buffer_dc != nullptr );

                ::ReleaseDC( m_window_handle, hdc );

                m_screen_buffer_bitmap_info.bmiHeader.biWidth = width;
                m_screen_buffer_bitmap_info.bmiHeader.biHeight = -height;
                m_screen_buffer_bitmap_info.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
                m_screen_buffer_bitmap_info.bmiHeader.biPlanes = 1;
                m_screen_buffer_bitmap_info.bmiHeader.biBitCount = 24;
                m_screen_buffer_bitmap_info.bmiHeader.biCompression = BI_RGB;
                m_screen_buffer_bitmap_info.bmiHeader.biXPelsPerMeter = 0x130B;
                m_screen_buffer_bitmap_info.bmiHeader.biYPelsPerMeter = 0x130B;

                RTL_ASSERT( m_screen_buffer_bitmap_handle == nullptr );

                m_screen_buffer_bitmap_handle
                    = ::CreateDIBSection( m_screen_buffer_dc,
                                          &m_screen_buffer_bitmap_info,
                                          DIB_RGB_COLORS,
                                          reinterpret_cast<void**>( &m_input.screen.pixels ),
                                          nullptr,
                                          0 );
                RTL_WINAPI_CHECK( m_screen_buffer_bitmap_handle != nullptr );

                constexpr size_t sizeof_rgb = 3;
                constexpr size_t align = sizeof( LONG );

                m_input.screen.pitch = ( ( ( sizeof_rgb * width ) + align - 1 ) / align ) * align;
            }

            void window::draw_screen_buffer( HDC hdc )
            {
                [[maybe_unused]] HGDIOBJ object
                    = ::SelectObject( m_screen_buffer_dc, m_screen_buffer_bitmap_handle );
                RTL_WINAPI_CHECK( object != nullptr );
                RTL_ASSERT( ::GetObjectType( object ) == OBJ_BITMAP );

                const int bitmap_width = m_screen_buffer_bitmap_info.bmiHeader.biWidth;
                const int bitmap_height = -m_screen_buffer_bitmap_info.bmiHeader.biHeight;

                RTL_ASSERT( bitmap_width <= width() );
                RTL_ASSERT( bitmap_height <= height() );

                [[maybe_unused]] BOOL result = ::BitBlt( hdc,
                                                         ( width() - bitmap_width ) / 2,
                                                         ( height() - bitmap_height ) / 2,
                                                         bitmap_width,
                                                         bitmap_height,
                                                         m_screen_buffer_dc,
                                                         0,
                                                         0,
                                                         SRCCOPY );
                RTL_WINAPI_CHECK( result );

                // TODO: This call brokes font rendering; deal with it later
                // object = ::SelectObject( that->m_screen_buffer_dc, object );
                // RTL_ASSERT( object == that->m_screen_buffer_bitmap_handle );
            }

            void window::free_screen_buffer()
            {
                if ( m_screen_buffer_dc )
                {
                    [[maybe_unused]] BOOL result = ::DeleteDC( m_screen_buffer_dc );
                    RTL_WINAPI_CHECK( result );
                    m_screen_buffer_dc = nullptr;
                }

                if ( m_screen_buffer_bitmap_handle )
                {
                    [[maybe_unused]] BOOL result = ::DeleteObject( m_screen_buffer_bitmap_handle );
                    RTL_WINAPI_CHECK( result );
                    m_screen_buffer_bitmap_handle = nullptr;
                }

                m_input.screen.pixels = nullptr;
                m_input.screen.pitch = 0;
            }

            void window::commit_screen_buffer()
            {
                [[maybe_unused]] BOOL result = ::InvalidateRect( m_window_handle, nullptr, FALSE );
                RTL_WINAPI_CHECK( result );
            }
        } // namespace win

    } // namespace impl
} // namespace rtl

    #endif
#endif