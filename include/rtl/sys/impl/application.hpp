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
#include <rtl/limits.hpp>
#include <rtl/memory.hpp>
#include <rtl/sys/application.hpp>
#include <rtl/sys/debug.hpp>
#include <rtl/vector.hpp>

#include "audio.hpp"
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
                [[nodiscard]] bool create( const wchar_t*              app_name,
                                           application::setup_function on_setup,
                                           application::init_function* on_init );

                void update( application::setup_function*  on_setup,
                             application::init_function*   on_resize,
                             application::update_function* on_update );

                int  width() const;
                int  height() const;
                bool fullscreen() const;

            private:
                void init_environment();

                SIZE initial_size() const;

                void destroy();

                void create_resizable_components( bool resize );
                void destroy_resizable_components( bool resize );

    #if RTL_ENABLE_APP_RESIZE
                void set_fullscreen_mode( bool fullscreen );
    #endif

    #if RTL_ENABLE_APP_AUDIO
                void create_audio();
                void commit_audio();
                void restart_audio();
                void destroy_audio();
    #endif

    #if RTL_ENABLE_APP_SCREEN_BUFFER
                void init_screen_buffer( int width, int height );
                void draw_screen_buffer( HDC hdc );
                void free_screen_buffer();
                void commit_screen_buffer();

        #if RTL_ENABLE_APP_OSD
                void init_osd_text( int width, int height );
                void draw_osd_text( HDC hdc );
                void free_osd_text();
        #endif
    #elif RTL_ENABLE_APP_OPENGL
                void  init_opengl( int width, int height );
                void  free_opengl();
                void  commit_opengl();
                void  enable_opengl_vsync();
    #endif
                static constexpr int  minimal_width = 600;
                static constexpr int  minimal_height = 400;
                static constexpr bool is_fullscreen = RTL_ENABLE_APP_FULLSCREEN;
                static constexpr bool is_resizable = RTL_ENABLE_APP_RESIZE;
                static constexpr bool has_cursor = RTL_ENABLE_APP_CURSOR;

                static constexpr DWORD fullscreen_style = WS_POPUP;
                static constexpr DWORD resizable_style = WS_OVERLAPPEDWINDOW;
                static constexpr DWORD fixed_style
                    = WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME;

                static LRESULT CALLBACK wnd_proc( HWND   hWnd,
                                                  UINT   uMsg,
                                                  WPARAM wParam,
                                                  LPARAM lParam );

                // NOTE: all variables must be initialized to zero
                //
                WNDCLASSW m_window_class{ 0 };
                HWND      m_window_handle{ nullptr };
                RECT      m_window_rect{ 0 };
                bool      m_window_inited{ false };
                bool      m_window_pad[3]{ false };

                application::input       m_input{ 0 };
                application::output      m_output{ 0 };
                application::params      m_params{ 0 };
                application::environment m_environment{ 0 };

    #if RTL_ENABLE_APP_RESIZE
                bool            m_resize_sizing{ false };
                bool            m_resize_sized{ false };
                bool            m_resize_fullscreen{ false };
                bool            m_resize_pad{ false };
                WINDOWPLACEMENT m_resize_placement{ 0 };
    #endif

    #if RTL_ENABLE_APP_AUDIO
                // NOTE: can't use \unique_ptr here because the \window class must have a trivial
                // constructor
                audio* m_audio{ nullptr };
    #endif

    #if RTL_ENABLE_APP_SCREEN_BUFFER
                HDC        m_screen_buffer_dc{ nullptr };
                BITMAPINFO m_screen_buffer_bitmap_info{ 0 };
                HBITMAP    m_screen_buffer_bitmap_handle{ nullptr };

        #if RTL_ENABLE_APP_OSD
                static constexpr auto osd_locations_count
                    = (size_t)application::output::osd::location::count;

                RECT  m_osd_rects[osd_locations_count]{ 0 };
                UINT  m_osd_params[osd_locations_count]{ 0 };
                HFONT m_osd_font{ nullptr };
        #endif
    #elif RTL_ENABLE_APP_OPENGL
                HGLRC m_opengl_rc_handle{ 0 };
                HDC   m_opengl_window_dc{ 0 };
    #endif
            };

            int window::width() const
            {
                return m_window_rect.right - m_window_rect.left;
            }

            int window::height() const
            {
                return m_window_rect.bottom - m_window_rect.top;
            }

            SIZE window::initial_size() const
            {
    #if !RTL_ENABLE_APP_RESIZE && RTL_ENABLE_APP_FULLSCREEN
                RECT window_rect;

                [[maybe_unused]] BOOL result
                    = ::GetWindowRect( ::GetDesktopWindow(), &window_rect );
                RTL_WINAPI_CHECK( result );

                return { window_rect.right - window_rect.left,
                         window_rect.bottom - window_rect.top };
    #else
                return { CW_USEDEFAULT, CW_USEDEFAULT };
    #endif
            }

            bool window::fullscreen() const
            {
    #if RTL_ENABLE_APP_RESIZE
                return m_resize_fullscreen;
    #else
                return RTL_ENABLE_APP_FULLSCREEN;
    #endif
            }

            bool window::create( const wchar_t*               app_name,
                                 application::setup_function* on_setup,
                                 application::init_function*  on_init )
            {
                m_window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
                m_window_class.lpfnWndProc = wnd_proc;
                m_window_class.lpszClassName = app_name;

                m_window_class.hInstance = ::GetModuleHandleW( nullptr );
                RTL_WINAPI_CHECK( m_window_class.hInstance != nullptr );

                m_window_class.hbrBackground = ( HBRUSH )::GetStockObject( BLACK_BRUSH );
                RTL_WINAPI_CHECK( m_window_class.hbrBackground != nullptr );

                if constexpr ( has_cursor || is_resizable )
                {
                    m_window_class.hCursor = ::LoadCursorW( nullptr, IDC_ARROW );
                    RTL_WINAPI_CHECK( m_window_class.hCursor != nullptr );
                }

                [[maybe_unused]] ATOM atom = ::RegisterClassW( &m_window_class );
                RTL_WINAPI_CHECK( atom != 0 );

                const SIZE size = initial_size();

                m_window_handle
                    = ::CreateWindowExW( WS_EX_APPWINDOW,
                                         m_window_class.lpszClassName,
                                         app_name,
                                         is_fullscreen && !is_resizable ? fullscreen_style
                                         : is_resizable                 ? resizable_style
                                                                        : fixed_style,
                                         CW_USEDEFAULT,
                                         CW_USEDEFAULT,
                                         size.cx,
                                         size.cy,
                                         nullptr,
                                         nullptr,
                                         m_window_class.hInstance,
                                         this );
                RTL_WINAPI_CHECK( m_window_handle != nullptr );
                m_input.window_handle = m_window_handle;

                init_environment();

                [[maybe_unused]] BOOL result;

                // NOTE: Application should always have the main window to ensure that singletone
                // feature works correctly. E.g. the setup callback can create additional dialog
                // windows and app have to reactivate them correctly after the double launch
                // attemption.
                if ( on_setup )
                {
                    if ( !on_setup( m_environment, m_params ) )
                    {
                        // launch is cancelled
                        result = ::DestroyWindow( m_window_handle );
                        RTL_WINAPI_CHECK( result );
                        return false;
                    }
                }

    #if RTL_ENABLE_APP_RESIZE || !RTL_ENABLE_APP_FULLSCREEN
                if ( m_params.window.width && m_params.window.height )
                {
                    result = ::SetWindowPos( m_window_handle,
                                             nullptr,
                                             0,
                                             0,
                                             m_params.window.width,
                                             m_params.window.height,
                                             SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER );
                    RTL_ASSERT( result );
                }
    #endif

    #if RTL_ENABLE_APP_RESIZE
                m_resize_placement.length = sizeof( m_resize_placement );

                if constexpr ( is_fullscreen )
                    set_fullscreen_mode( true );
    #else
                m_window_inited = true;
    #endif
                create_resizable_components( false );

    #if RTL_ENABLE_APP_AUDIO
                create_audio();
    #endif
                ::ShowWindow( m_window_handle, SW_SHOW );

                result = ::UpdateWindow( m_window_handle );
                RTL_WINAPI_CHECK( result );

                if ( on_init )
                    on_init( m_environment, m_input );

                return true;
            }

            void window::create_resizable_components( [[maybe_unused]] bool resize )
            {
                [[maybe_unused]] BOOL result = ::GetClientRect( m_window_handle, &m_window_rect );
                RTL_WINAPI_CHECK( result );

                const int width = this->width();
                const int height = this->height();

                m_input.screen.width = width;
                m_input.screen.height = height;

    #if RTL_ENABLE_APP_SCREEN_BUFFER
                init_screen_buffer( width, height );
        #if RTL_ENABLE_APP_OSD
                init_osd_text( width, height );
        #endif
    #elif RTL_ENABLE_APP_OPENGL
                if ( !resize )
                    init_opengl( width, height );
    #endif
            }

            void window::destroy_resizable_components( [[maybe_unused]] bool resize )
            {
    #if RTL_ENABLE_APP_SCREEN_BUFFER
        #if RTL_ENABLE_APP_OSD
                free_osd_text();
        #endif
                free_screen_buffer();
    #elif RTL_ENABLE_APP_OPENGL
                if ( !resize )
                    free_opengl();
    #endif
            }

            void window::destroy()
            {
                m_window_inited = false;

                destroy_resizable_components( false );

    #if RTL_ENABLE_APP_AUDIO
                destroy_audio();
    #endif

                // NOTE: Non-critical for the application beying terminated
                // ::UnregisterClassW( m_window_class.lpszClassName, m_window_class.hInstance );
            }

            void window::update( [[maybe_unused]] application::setup_function* on_setup,
                                 [[maybe_unused]] application::init_function*  on_init,
                                 application::update_function*                 on_update )
            {
                if ( !m_window_inited )
                    return;

                [[maybe_unused]] BOOL result = ::GdiFlush();
                RTL_WINAPI_CHECK( result );

    #if RTL_ENABLE_APP_CLOCK
                m_input.clock.thirds = static_cast<int32_t>( ::GetTickCount() )
                                       * application::input::clock::measure / 1000;
    #endif

    #if RTL_ENABLE_APP_RESIZE
                if ( m_resize_sized )
                {
                    destroy_resizable_components( true );
                    create_resizable_components( true );
        #if RTL_ENABLE_APP_AUDIO
                    restart_audio();
        #endif
                    if ( on_init )
                        on_init( m_environment, m_input );

                    m_resize_sized = false;
                }
    #endif

                const auto action
                    = on_update ? on_update( m_input, m_output ) : application::action::none;

    #if RTL_ENABLE_APP_KEYS
                rtl::fill_n( m_input.keys.pressed, (size_t)keyboard::keys::count, false );
    #endif

                switch ( action )
                {
                case application::action::close:
                    result = ::DestroyWindow( m_window_handle );
                    break;

    #if RTL_ENABLE_APP_RESIZE
                case application::action::toggle_fullscreen:
                    set_fullscreen_mode( !m_resize_fullscreen );
                    break;
    #endif
    #if RTL_ENABLE_APP_RESET
                case application::action::reset:
                {
                    if ( on_setup )
                    {
        #if RTL_ENABLE_APP_OPENGL
                        // NOTE: Hack listed here
                        // https://stackoverflow.com/questions/2378918/modal-dialogs-opened-by-a-fullscreen-opengl-window-on-windows-7-are-not-showing
                        result = ::RedrawWindow( m_window_handle, 0, 0, RDW_INTERNALPAINT );
                        RTL_WINAPI_CHECK( result );
        #endif
                        if ( on_setup( m_environment, m_params ) )
                        {
        #if RTL_ENABLE_APP_AUDIO
                            destroy_audio();
                            create_audio();
        #endif

        #if RTL_ENABLE_APP_RESIZE || !RTL_ENABLE_APP_FULLSCREEN
                                    // TODO: If fixed size window -> resize window and fetch
                                    // m_resize_sized event
                                    // TODO: If resizable window but fullscreen mode -> update
                                    // window size into set_fullscreen_mode
                                    // TODO: If resizable window in windowed mode -> update
                                    // window size immediately
        #endif

                            // NOTE: \on_init should always be called immediatelly after \on_setup
                            if ( on_init )
                                on_init( m_environment, m_input );
                        }
                    }

                    break;
                }
    #endif

                case application::action::none:
                default:
    #if RTL_ENABLE_APP_SCREEN_BUFFER
                    commit_screen_buffer();
    #elif RTL_ENABLE_APP_OPENGL
                    commit_opengl();
    #endif
    #if RTL_ENABLE_APP_AUDIO
                    commit_audio();
    #endif
                    break;
                }
            }

            window g_window;

        } // namespace win

    } // namespace impl

    void application::run( const wchar_t*      app_name,
                           setup_function*     on_setup,
                           init_function*      on_init,
                           update_function*    on_update,
                           terminate_function* on_terminate )
    {
    #if RTL_ENABLE_APP_SINGLETON
        rtl::unique_ptr<void, decltype( &::CloseHandle )> mutex(
            ::OpenMutexW( SYNCHRONIZE, FALSE, app_name ), ::CloseHandle );

        if ( mutex )
        {
            // Find application's main window (can be hidden)
            HWND window = ::FindWindowW( app_name, nullptr );
            if ( window != nullptr )
            {
                // Main window ownes popup window (e.g. dialog box)
                if ( HWND dialog = ::GetWindow( window, GW_ENABLEDPOPUP ) )
                    window = dialog;

                // Main window is hidden, but has sibling window (e.g. dialog box)
                if ( !::IsWindowVisible( window ) )
                    window = ::GetWindow( window, GW_HWNDPREV );

                // Dialog box can has children message boxes
                if ( HWND dialog = ::GetWindow( window, GW_ENABLEDPOPUP ) )
                    window = dialog;

                // Anyway, do not touch hidden windows
                if ( ::IsWindowVisible( window ) )
                {
                    ::ShowWindow( window, SW_SHOWNOACTIVATE );
                    ::SetForegroundWindow( window );
                }
            }

            return;
        }

        mutex.reset( ::CreateMutexW( nullptr, FALSE, app_name ) );
        RTL_WINAPI_CHECK( mutex != nullptr );
    #endif

        if ( !impl::win::g_window.create( app_name, on_setup, on_init ) )
            return;

        MSG msg{ 0 };

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
            impl::win::g_window.update( on_setup, on_init, on_update );
        }

        if ( on_terminate )
            on_terminate();
    }

    application& application::instance()
    {
        static application g_app;
        return g_app;
    }
} // namespace rtl

#endif
