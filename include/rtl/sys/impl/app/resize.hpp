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
            void window::set_fullscreen_mode( bool fullscreen )
            {
                m_resize_fullscreen = fullscreen;

                [[maybe_unused]] BOOL result;

                if ( m_resize_fullscreen )
                {
                    result = ::GetWindowPlacement( m_window_handle, &m_resize_placement );
                    RTL_WINAPI_CHECK( result );

                    [[maybe_unused]] LONG res = ::SetWindowLongW(
                        m_window_handle,
                        GWL_STYLE,
                        (LONG)( m_resize_fullscreen ? fullscreen_style : resizable_style ) );
                    RTL_WINAPI_CHECK( res != 0 );

                    RECT rect;
                    result = ::GetWindowRect( ::GetDesktopWindow(), &rect );
                    RTL_WINAPI_CHECK( result );

                    result = ::SetWindowPos( m_window_handle,
                                             HWND_TOP,
                                             0,
                                             0,
                                             rect.right - rect.left,
                                             rect.bottom - rect.top,
                                             SWP_SHOWWINDOW );
                    RTL_WINAPI_CHECK( result );
                }
                else
                {
                    [[maybe_unused]] LONG res = ::SetWindowLongW(
                        m_window_handle,
                        GWL_STYLE,
                        (LONG)( m_resize_fullscreen ? fullscreen_style : resizable_style ) );
                    RTL_WINAPI_CHECK( res != 0 );

                    result = ::SetWindowPlacement( m_window_handle, &m_resize_placement );
                    RTL_WINAPI_CHECK( result );

                    // TODO: change size if window size doesn't match m_params
                }
            }

        } // namespace win

    } // namespace impl
} // namespace rtl

    #endif
#endif