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
    #if RTL_ENABLE_APP_OPENGL

        #if RTL_ENABLE_APP_SCREEN_BUFFER
            #error "RTL_ENABLE_APP_OPENGL and RTL_ENABLE_APP_SCREEN_BUFFER are mutually exclusive"
        #endif

namespace rtl
{
    namespace impl
    {
        namespace win
        {
            void window::init_opengl( [[maybe_unused]] int width, [[maybe_unused]] int height )
            {
                m_window_dc = ::GetDC( m_window_handle );
                RTL_WINAPI_CHECK( m_window_dc != nullptr );

                PIXELFORMATDESCRIPTOR pfd{ 0 };

                pfd.nSize = static_cast<WORD>( sizeof( PIXELFORMATDESCRIPTOR ) );
                pfd.nVersion = 1;
                pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER
                              | PFD_DEPTH_DONTCARE;
                pfd.iPixelType = PFD_TYPE_RGBA;
                pfd.cColorBits = 32;

                const int pixelFormat = ::ChoosePixelFormat( m_window_dc, &pfd );
                RTL_WINAPI_CHECK( pixelFormat != 0 );

                [[maybe_unused]] BOOL result = ::SetPixelFormat( m_window_dc, pixelFormat, &pfd );
                RTL_WINAPI_CHECK( result );

                m_glrc_handle = ::wglCreateContext( m_window_dc );
                RTL_WINAPI_CHECK( m_glrc_handle != nullptr );

                result = ::wglMakeCurrent( m_window_dc, m_glrc_handle );
                RTL_WINAPI_CHECK( result );
            }

            void window::free_opengl()
            {
                if ( m_glrc_handle )
                {
                    [[maybe_unused]] BOOL result = ::wglMakeCurrent( nullptr, nullptr );
                    RTL_WINAPI_CHECK( result );

                    result = ::wglDeleteContext( m_glrc_handle );
                    RTL_WINAPI_CHECK( result );

                    m_glrc_handle = nullptr;
                }

                if ( m_window_dc )
                {
                    ::ReleaseDC( m_window_handle, m_window_dc );
                    m_window_dc = nullptr;
                }
            }

            void window::commit_opengl()
            {
                ::SwapBuffers( m_window_dc );
            }

        } // namespace win

    } // namespace impl
} // namespace rtl

    #endif
#endif
