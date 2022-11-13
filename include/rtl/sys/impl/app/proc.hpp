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

#include <rtl/sys/impl/application.hpp>

#if RTL_ENABLE_APP
    #if RTL_ENABLE_APP_RESIZE

namespace rtl
{
    namespace impl
    {
        namespace win
        {
            LRESULT window::wnd_proc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
            {
                window* that
                    = reinterpret_cast<window*>( ::GetWindowLongPtrW( hWnd, GWL_USERDATA ) );

                switch ( uMsg )
                {
                case WM_CLOSE:
                {
                    [[maybe_unused]] BOOL result = ::DestroyWindow( that->m_window_handle );
                    return 0;
                }

                case WM_DESTROY:
                    that->destroy();
                    ::PostQuitMessage( 0 );
                    return 0;

        #if RTL_ENABLE_APP_KEYS
                case WM_KEYDOWN:
                {
                    const int key = static_cast<int>( virtual_key_to_enum( wParam ) );

                    if ( !that->m_input.keys.state[key] )
                        that->m_input.keys.pressed[key] = true;

                    that->m_input.keys.state[key] = true;
                    return 0;
                }

                case WM_KEYUP:
                {
                    const int key = static_cast<int>( virtual_key_to_enum( wParam ) );
                    that->m_input.keys.state[key] = false;
                    return 0;
                }
        #endif

        #if RTL_ENABLE_APP_RESIZE
                case WM_SIZING:
                {
                    if ( that )
                        that->m_sizing = true;

                    return 0;
                }

                case WM_EXITSIZEMOVE:
                {
                    if ( that )
                        that->m_sizing = false;

                    break;
                }

                case WM_SIZE:
                {
                    if ( wParam != SIZE_MINIMIZED )
                    {
                        if ( that )
                            that->m_sized = true;
                    }

                    break;
                }

                case WM_GETMINMAXINFO:
                {
                    if ( that )
                    {
                        MINMAXINFO* minmax = reinterpret_cast<MINMAXINFO*>( lParam );
                        minmax->ptMinTrackSize.x = minimal_width;
                        minmax->ptMinTrackSize.y = minimal_height;
                    }

                    break;
                }

        #endif

        #if RTL_ENABLE_APP_SCREEN_BUFFER
                case WM_PAINT:
                {
            #if RTL_ENABLE_APP_RESIZE
                    if ( that->m_sizing )
                        break;
            #endif
                    PAINTSTRUCT ps;

                    HDC hdc = ::BeginPaint( hWnd, &ps );
                    RTL_WINAPI_CHECK( hdc != nullptr );

            #if RTL_ENABLE_APP_SCREEN_BUFFER
                #if RTL_ENABLE_APP_OSD
                    that->draw_osd_text( hdc );
                #endif
                    that->draw_screen_buffer( hdc );
            #endif

                    [[maybe_unused]] BOOL result = ::EndPaint( hWnd, &ps );
                    RTL_WINAPI_CHECK( result );
                    return 0;
                }
        #endif

                case WM_SETCURSOR:
                    if constexpr ( !has_cursor || is_resizable )
                    {
                        if ( LOWORD( lParam ) == HTCLIENT )
                        {
                            ::SetCursor( !has_cursor || that->fullscreen()
                                             ? nullptr
                                             : that->m_window_class.hCursor );
                            return TRUE;
                        }
                    }

                    break;
                }

                return ::DefWindowProcW( hWnd, uMsg, wParam, lParam );
            }

        } // namespace win

    } // namespace impl
} // namespace rtl

    #endif
#endif