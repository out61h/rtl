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
                case WM_CREATE:
                {
                    CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>( lParam );
                    ::SetWindowLongPtrW(
                        hWnd, GWL_USERDATA, reinterpret_cast<LONG_PTR>( cs->lpCreateParams ) );

                    return 0;
                }

                case WM_CLOSE:
                {
                    [[maybe_unused]] BOOL result = ::DestroyWindow( that->m_window_handle );
                    RTL_WINAPI_CHECK( result );
                    return 0;
                }

                case WM_DESTROY:
                    that->destroy();
                    ::PostQuitMessage( 0 );
                    return 0;

                case WM_SYSCOMMAND:
                    // Disable window menu call by F10 key
                    if ( ( wParam & 0xfff0 ) == SC_KEYMENU )
                        return 0;

                    break;

    #if RTL_ENABLE_APP_KEYS
                case WM_ACTIVATE:
                    rtl::fill_n( that->m_input.keys.state, (size_t)keyboard::Keys::count, false );
                    return 0;

                case WM_SYSKEYDOWN:
                case WM_KEYDOWN:
                {
                    const unsigned key = static_cast<unsigned>( wParam );

                    if ( !that->m_input.keys.state[key] )
                        that->m_input.keys.pressed[key] = true;

                    that->m_input.keys.state[key] = true;
                    return 0;
                }

                case WM_SYSKEYUP:
                case WM_KEYUP:
                {
                    const unsigned key = static_cast<unsigned>( wParam );
                    that->m_input.keys.state[key] = false;
                    return 0;
                }
    #endif

    #if RTL_ENABLE_APP_RESIZE
                case WM_SIZING:
                {
                    that->m_resize_sizing = true;
                    return 0;
                }

        #if RTL_ENABLE_APP_AUDIO_OUTPUT

                case WM_ENTERSIZEMOVE:
                {
                    that->m_audio->stop();
                    return 0;
                }
        #endif

                case WM_EXITSIZEMOVE:
                {
                    that->m_resize_sizing = false;
                    return 0;
                }

                case WM_SIZE:
                {
                    if ( wParam != SIZE_MINIMIZED )
                    {
                        that->m_resize_sized = that->m_window_inited;
                        that->m_window_inited = true;
                    }

                    return 0;
                }

                case WM_GETMINMAXINFO:
                {
                    MINMAXINFO* minmax = reinterpret_cast<MINMAXINFO*>( lParam );
                    minmax->ptMinTrackSize.x = minimal_width;
                    minmax->ptMinTrackSize.y = minimal_height;

                    return 0;
                }
    #endif

    #if RTL_ENABLE_APP_SCREEN_BUFFER
                case WM_PAINT:
                {
        #if RTL_ENABLE_APP_RESIZE
                    if ( that->m_resize_sizing )
                        break;
        #endif
                    PAINTSTRUCT ps;

                    HDC hdc = ::BeginPaint( hWnd, &ps );
                    RTL_WINAPI_CHECK( hdc != nullptr );

        #if RTL_ENABLE_APP_OSD
                    that->draw_osd_text( hdc );
        #endif
                    that->draw_screen_buffer( hdc );

                    [[maybe_unused]] BOOL result = ::EndPaint( hWnd, &ps );
                    RTL_WINAPI_CHECK( result );
                    return 0;
                }
    #else
                case WM_PAINT:
                {
        #if RTL_ENABLE_APP_RESIZE
                    if ( that->m_resize_sizing )
                        break;
        #endif
                    // NOTE: Hack listed here:
                    // https://stackoverflow.com/questions/2378918/modal-dialogs-opened-by-a-fullscreen-opengl-window-on-windows-7-are-not-showing
                    // Also helps to eliminate black invalidated areas on window after moving it out
                    // of the desktop
                    PAINTSTRUCT ps;

                    HDC hdc = ::BeginPaint( hWnd, &ps );
                    RTL_WINAPI_CHECK( hdc != nullptr );
                    ::SwapBuffers( hdc );
                    [[maybe_unused]] BOOL result = ::EndPaint( hWnd, &ps );
                    RTL_WINAPI_CHECK( result );
                    break;
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

    }     // namespace impl
} // namespace rtl

#endif