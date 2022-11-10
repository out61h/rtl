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

#include <rtl/algorithm.hpp>
#include <rtl/sys/application.hpp>
#include <rtl/sys/debug.hpp>

#include "keyboard.hpp"
#include "memory.hpp"
#include "win.hpp"

#if RTL_ENABLE_APP

namespace rtl
{
    namespace impl
    {
        namespace win
        {
            class window final
            {
            public:
                void create( const wchar_t* window_name );
                void update( application::update_function* update_fn );
                void invalidate();
                void destroy();

                int width() const;
                int height() const;

            private:
                static constexpr bool is_fullscreen = RTL_ENABLE_APP_FULLSCREEN;
                static constexpr bool is_fixed_size = RTL_ENABLE_APP_FIXED_WINDOW_SIZE;
                static constexpr bool has_cursor = RTL_ENABLE_APP_CURSOR;

                static constexpr DWORD style
                    = is_fullscreen
                          ? WS_POPUP
                          : ( is_fixed_size ? WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME
                                            : WS_OVERLAPPEDWINDOW );

                static LRESULT CALLBACK wnd_proc( HWND   hWnd,
                                                  UINT   uMsg,
                                                  WPARAM wParam,
                                                  LPARAM lParam );

                WNDCLASSW m_window_class{ 0 };

                BITMAPINFO m_bitmap_info{ 0 };

                HDC     m_device_context_handle{ nullptr };
                HWND    m_window_handle{ nullptr };
                HBITMAP m_bitmap_handle{ nullptr };
                HFONT   m_font{ nullptr };

                RECT m_client_rect{ 0 };

                application::input  m_input{ 0 };
                application::output m_output{ 0 };

    #if RTL_ENABLE_APP_OSD
                static constexpr auto osd_locations_count
                    = (size_t)application::output::osd::location::count;

                RECT m_osd_rects[osd_locations_count]{ 0 };
                UINT m_osd_params[osd_locations_count]{ 0 };
    #endif
            };

            int window::width() const
            {
                return m_client_rect.right - m_client_rect.left;
            }

            int window::height() const
            {
                return m_client_rect.bottom - m_client_rect.top;
            }

            void window::create( const wchar_t* window_name )
            {
                m_window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
                m_window_class.lpfnWndProc = wnd_proc;
                m_window_class.lpszClassName = L".";

                m_window_class.hInstance = ::GetModuleHandleW( nullptr );
                RTL_WINAPI_CHECK( m_window_class.hInstance != nullptr );

                m_window_class.hbrBackground = ( HBRUSH )::GetStockObject( BLACK_BRUSH );
                RTL_WINAPI_CHECK( m_window_class.hbrBackground != nullptr );

                if ( has_cursor )
                {
                    m_window_class.hCursor = ::LoadCursorW( nullptr, IDC_ARROW );
                    RTL_WINAPI_CHECK( m_window_class.hCursor != nullptr );
                }

                [[maybe_unused]] ATOM atom = ::RegisterClassW( &m_window_class );
                RTL_WINAPI_CHECK( atom != 0 );

                BOOL result;
                RECT window_rect;

                if ( is_fullscreen )
                {
                    result = ::GetWindowRect( ::GetDesktopWindow(), &window_rect );
                    RTL_WINAPI_CHECK( result );
                }

                m_window_handle = ::CreateWindowExW(
                    WS_EX_APPWINDOW,
                    m_window_class.lpszClassName,
                    window_name,
                    style,
                    CW_USEDEFAULT,
                    CW_USEDEFAULT,
                    is_fullscreen ? window_rect.right - window_rect.left : CW_USEDEFAULT,
                    is_fullscreen ? window_rect.bottom - window_rect.top : CW_USEDEFAULT,
                    nullptr,
                    nullptr,
                    m_window_class.hInstance,
                    nullptr ); // TODO: this?
                RTL_WINAPI_CHECK( m_window_handle != nullptr );

                static_assert( sizeof( this ) == sizeof( LONG_PTR ) );
                ::SetWindowLongPtrW(
                    m_window_handle, GWL_USERDATA, reinterpret_cast<LONG_PTR>( this ) );

                // TODO: Check order of subsequent calls
                ::ShowWindow( m_window_handle, SW_SHOW );

                result = ::UpdateWindow( m_window_handle );
                RTL_WINAPI_CHECK( result );

                HDC hdc = ::GetDC( m_window_handle );
                RTL_WINAPI_CHECK( hdc != nullptr );

                m_device_context_handle = ::CreateCompatibleDC( hdc );
                RTL_WINAPI_CHECK( m_device_context_handle != nullptr );

                ::ReleaseDC( m_window_handle, hdc );

                result = ::GetClientRect( m_window_handle, &m_client_rect );
                RTL_WINAPI_CHECK( result );

                const int width = this->width();
                const int height = this->height();

                {
                    m_bitmap_info.bmiHeader.biWidth = width;
                    m_bitmap_info.bmiHeader.biHeight = -height;
                    m_bitmap_info.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
                    m_bitmap_info.bmiHeader.biPlanes = 1;
                    m_bitmap_info.bmiHeader.biBitCount = 24;
                    m_bitmap_info.bmiHeader.biCompression = BI_RGB;
                    m_bitmap_info.bmiHeader.biXPelsPerMeter = 0x130B;
                    m_bitmap_info.bmiHeader.biYPelsPerMeter = 0x130B;

                    m_bitmap_handle
                        = ::CreateDIBSection( m_device_context_handle,
                                              &m_bitmap_info,
                                              DIB_RGB_COLORS,
                                              reinterpret_cast<void**>( &m_output.screen.pixels ),
                                              nullptr,
                                              0 );
                    RTL_WINAPI_CHECK( m_bitmap_handle != nullptr );

                    constexpr size_t sizeof_rgb = 3;
                    constexpr size_t align = sizeof( LONG );

                    m_output.screen.pitch
                        = ( ( ( sizeof_rgb * width ) + align - 1 ) / align ) * align;
                }

                ::SelectObject( m_device_context_handle, m_bitmap_handle );

                m_output.screen.width = width;
                m_output.screen.height = height;

    #if RTL_ENABLE_APP_OSD
                {
                    m_font = ::CreateFontW( application::output::osd::font_size * width / 1280,
                                            0,
                                            0,
                                            0,
                                            FW_DONTCARE,
                                            FALSE,
                                            FALSE,
                                            FALSE,
                                            DEFAULT_CHARSET,
                                            OUT_DEFAULT_PRECIS,
                                            CLIP_DEFAULT_PRECIS,
                                            DEFAULT_QUALITY,
                                            DEFAULT_PITCH | FF_DONTCARE,
                                            NULL );
                    ::SelectObject( m_device_context_handle, m_font );

                    // NOTE: The same color as background one.
                    ::SetBkColor( m_device_context_handle, RGB( 0, 0, 0 ) );
                    // ::SetBkMode( m_device_context_handle, TRANSPARENT );
                    ::SetTextColor( m_device_context_handle, RGB( 255, 255, 255 ) );

                    TEXTMETRIC tm;
                    result = ::GetTextMetricsW( m_device_context_handle, &tm );
                    RTL_WINAPI_CHECK( result );

                    const int     font_height = tm.tmHeight;
                    constexpr int osd_margin = application::output::osd::margin;
                    const int     osd_height = 2 * osd_margin + font_height;

                    constexpr size_t i0 = (size_t)application::output::osd::location::top_left;
                    m_osd_rects[i0].left = osd_margin;
                    m_osd_rects[i0].right = width - osd_margin;
                    m_osd_rects[i0].top = osd_margin;
                    m_osd_rects[i0].bottom = osd_height;
                    m_osd_params[i0] = DT_TOP | DT_LEFT | DT_NOCLIP;

                    constexpr size_t i1 = (size_t)application::output::osd::location::top_right;
                    m_osd_rects[i1].left = width / 2 + osd_margin;
                    m_osd_rects[i1].right = width - osd_margin;
                    m_osd_rects[i1].top = osd_margin;
                    m_osd_rects[i1].bottom = osd_height;
                    m_osd_params[i1] = DT_TOP | DT_RIGHT | DT_NOCLIP;

                    constexpr size_t i2 = (size_t)application::output::osd::location::bottom_left;
                    m_osd_rects[i2].left = osd_margin;
                    m_osd_rects[i2].right = width - osd_margin;
                    m_osd_rects[i2].top = height - font_height - osd_margin;
                    m_osd_rects[i2].bottom = height - osd_margin;
                    m_osd_params[i2] = DT_TOP | DT_LEFT | DT_NOCLIP;

                    constexpr size_t i3 = (size_t)application::output::osd::location::bottom_right;
                    m_osd_rects[i3].left = width / 2 + osd_margin;
                    m_osd_rects[i3].right = width - osd_margin;
                    m_osd_rects[i3].top = height - font_height - osd_margin;
                    m_osd_rects[i3].bottom = height - osd_margin;
                    m_osd_params[i3] = DT_TOP | DT_RIGHT | DT_NOCLIP;

                    m_output.screen.pixels += m_output.screen.pitch * osd_height;
                    m_output.screen.height -= osd_height * 2;
                }
    #endif
            }

            void window::destroy()
            {
                BOOL result;
                result = ::DeleteDC( m_device_context_handle );
                RTL_WINAPI_CHECK( result );

                result = ::DeleteObject( m_font );
                RTL_WINAPI_CHECK( result );

                if ( m_bitmap_handle )
                {
                    result = ::DeleteObject( m_bitmap_handle );
                    RTL_WINAPI_CHECK( result );
                }

                result = ::DestroyWindow( m_window_handle );
                RTL_WINAPI_CHECK( result );
            }

            LRESULT window::wnd_proc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
            {
                if ( uMsg == WM_CLOSE )
                {
                    ::PostQuitMessage( 0 );
                    return 0;
                }

                window* that
                    = reinterpret_cast<window*>( ::GetWindowLongPtrW( hWnd, GWL_USERDATA ) );

                switch ( uMsg )
                {
    #if RTL_ENABLE_APP_KEYS
                case WM_KEYDOWN:
                {
                    const int key = static_cast<int>( virtual_key_to_enum( wParam ) );

                    if ( !that->m_input.keys.state[key] )
                        that->m_input.keys.pressed[key] = true;

                    that->m_input.keys.state[key] = true;
                    break;
                }

                case WM_KEYUP:
                {
                    const int key = static_cast<int>( virtual_key_to_enum( wParam ) );
                    that->m_input.keys.state[key] = false;
                    break;
                }
    #endif
                case WM_PAINT:
                {
                    PAINTSTRUCT ps;

                    HDC hdc = ::BeginPaint( hWnd, &ps );
                    RTL_WINAPI_CHECK( hdc != nullptr );

                    if ( that->m_device_context_handle )
                    {
                        const int width = that->m_bitmap_info.bmiHeader.biWidth;
                        const int height = -that->m_bitmap_info.bmiHeader.biHeight;

                        RTL_ASSERT( width <= that->width() );
                        RTL_ASSERT( height <= that->height() );

                        [[maybe_unused]] BOOL result = ::BitBlt( hdc,
                                                                 ( that->width() - width ) / 2,
                                                                 ( that->height() - height ) / 2,
                                                                 width,
                                                                 height,
                                                                 that->m_device_context_handle,
                                                                 0,
                                                                 0,
                                                                 SRCCOPY );
                        RTL_WINAPI_CHECK( result );
                    }

                    [[maybe_unused]] BOOL result = ::EndPaint( hWnd, &ps );
                    RTL_WINAPI_CHECK( result );
                    return 0;
                }
                case WM_SETCURSOR:
                    if ( !has_cursor )
                    {
                        if ( LOWORD( lParam ) == HTCLIENT )
                        {
                            SetCursor( nullptr );
                            return TRUE;
                        }
                    }

                    break;
                }
                return ::DefWindowProcW( hWnd, uMsg, wParam, lParam );
            }

            void window::invalidate()
            {
                [[maybe_unused]] BOOL result = ::InvalidateRect( m_window_handle, nullptr, FALSE );
                RTL_WINAPI_CHECK( result );
            }

            void window::update( application::update_function* fn )
            {
                [[maybe_unused]] BOOL result = ::GdiFlush();
                RTL_WINAPI_CHECK( result );

    #if RTL_ENABLE_APP_CLOCK
                m_input.clock.thirds = static_cast<int32_t>( ::GetTickCount() )
                                       * application::input::clock::measure / 1000;
    #endif

                auto action = fn( m_input, m_output );

    #if RTL_ENABLE_APP_KEYS
                for ( size_t i = 0; i < static_cast<size_t>( keyboard::keys::count ); ++i )
                    m_input.keys.pressed[i] = false;
    #endif

    #if RTL_ENABLE_APP_OSD
                for ( int i = 0; i < osd_locations_count; ++i )
                {
                    [[maybe_unused]] const int res = ::FillRect(
                        m_device_context_handle, &m_osd_rects[i], m_window_class.hbrBackground );
                    RTL_WINAPI_CHECK( res != 0 );
                }

                for ( int i = 0; i < osd_locations_count; ++i )
                {
                    [[maybe_unused]] const int res = ::DrawTextW( m_device_context_handle,
                                                                  m_output.osd.text[i],
                                                                  -1,
                                                                  &m_osd_rects[i],
                                                                  m_osd_params[i] | DT_SINGLELINE );
                    RTL_WINAPI_CHECK( res != 0 );
                }
    #endif
                if ( action == application::action::close )
                {
                    ::PostQuitMessage( 0 );
                }
                else
                {
                    invalidate();
                }
            }

            window g_window;

        } // namespace win

    } // namespace impl

    void application::run( const wchar_t* app_name, update_function* update_fn )
    {
        impl::win::g_window.create( app_name );

        MSG msg{ 0 };

    #if 0
        for ( BOOL ret; ( ret = ::GetMessageW( &msg, nullptr, 0, 0 ) ) != 0; )
        {
            ::TranslateMessage( &msg );
            ::DispatchMessageW( &msg ); // TODO: result => exit or load next
        }
    #else
        for ( ; msg.message != WM_QUIT; )
        {
            while ( ::PeekMessageW( &msg, nullptr, 0, 0, PM_REMOVE ) )
            {
                if ( msg.message == WM_QUIT )
                    break;

                ::TranslateMessage( &msg );
                ::DispatchMessageW( &msg );
            }

            // TODO: wait for signal to update
            // TODO: run processing in separate thread
            // TODO: return time for next start or -1 for infinite wait
            impl::win::g_window.update( update_fn );
        }
    #endif

        impl::win::g_window.destroy();
    }

    application& application::instance()
    {
        static application g_app;
        return g_app;
    }
} // namespace rtl

#endif
