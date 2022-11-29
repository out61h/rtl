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

#include <rtl/string.hpp>

#include <rtl/sys/impl/application.hpp>

#include <gl/GL.h>

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
                m_opengl_window_dc = ::GetDC( m_window_handle );
                RTL_WINAPI_CHECK( m_opengl_window_dc != nullptr );

                PIXELFORMATDESCRIPTOR pfd{ 0 };

                pfd.nSize = static_cast<WORD>( sizeof( PIXELFORMATDESCRIPTOR ) );
                pfd.nVersion = 1;
                pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER
                              | PFD_DEPTH_DONTCARE;
                pfd.iPixelType = PFD_TYPE_RGBA;
                pfd.cColorBits = 32;

                const int pixelFormat = ::ChoosePixelFormat( m_opengl_window_dc, &pfd );
                RTL_WINAPI_CHECK( pixelFormat != 0 );

                [[maybe_unused]] BOOL result
                    = ::SetPixelFormat( m_opengl_window_dc, pixelFormat, &pfd );
                RTL_WINAPI_CHECK( result );

                m_opengl_rc_handle = ::wglCreateContext( m_opengl_window_dc );
                RTL_WINAPI_CHECK( m_opengl_rc_handle != nullptr );

                result = ::wglMakeCurrent( m_opengl_window_dc, m_opengl_rc_handle );
                RTL_WINAPI_CHECK( result );

        #if RTL_ENABLE_APP_OPENGL_VSYNC
                enable_opengl_vsync();
        #endif
            }

            void window::free_opengl()
            {
                if ( m_opengl_rc_handle )
                {
                    [[maybe_unused]] BOOL result = ::wglMakeCurrent( nullptr, nullptr );
                    RTL_WINAPI_CHECK( result );

                    result = ::wglDeleteContext( m_opengl_rc_handle );
                    RTL_WINAPI_CHECK( result );

                    m_opengl_rc_handle = nullptr;
                }

                if ( m_opengl_window_dc )
                {
                    ::ReleaseDC( m_window_handle, m_opengl_window_dc );
                    m_opengl_window_dc = nullptr;
                }
            }

            void window::commit_opengl()
            {
                ::SwapBuffers( m_opengl_window_dc );
            }

            void window::enable_opengl_vsync()
            {
                typedef BOOL( APIENTRY * PFNWGLSWAPINTERVALPROC )( int );

                const char* const extensions
                    = reinterpret_cast<const char*>( ::glGetString( GL_EXTENSIONS ) );
                RTL_ASSERT( extensions != nullptr );

                rtl::string_view ext( extensions );

                if ( ext.find( "WGL_EXT_swap_control" ) != rtl::string_view::npos )
                {
                    void* const address = ::wglGetProcAddress( "wglSwapIntervalEXT" );
                    RTL_ASSERT( address != nullptr );

                    if ( auto wglSwapIntervalEXT = static_cast<PFNWGLSWAPINTERVALPROC>( address ) )
                    {
                        wglSwapIntervalEXT( 1 );

                        RTL_LOG( "ON" );
                    }
                }
            }

        } // namespace win

    } // namespace impl
} // namespace rtl

    #endif
#endif
