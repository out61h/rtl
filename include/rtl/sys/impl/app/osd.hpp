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
    #if RTL_ENABLE_APP_OSD
        #if !RTL_ENABLE_APP_SCREEN_BUFFER
            #error "RTL_ENABLE_APP_OSD=1 needs RTL_ENABLE_APP_SCREEN_BUFFER=1"
        #endif

namespace rtl
{
    namespace impl
    {
        namespace win
        {
            void window::init_osd_text( int width, int height )
            {
                RTL_ASSERT( m_osd_font == nullptr );

                m_osd_font = ::CreateFontW( Application::Output::OSD::font_size * width / 1280,
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
                                            nullptr );
                HGDIOBJ object = ::SelectObject( m_screen_buffer_dc, m_osd_font );
                RTL_WINAPI_CHECK( object != nullptr );
                RTL_ASSERT( ::GetObjectType( object ) == OBJ_FONT );

                TEXTMETRIC tm;

                [[maybe_unused]] BOOL result = ::GetTextMetricsW( m_screen_buffer_dc, &tm );
                RTL_WINAPI_CHECK( result );

                object = ::SelectObject( m_screen_buffer_dc, object );
                RTL_ASSERT( object == m_osd_font );

                const int     font_height = tm.tmHeight;
                constexpr int osd_margin = Application::Output::OSD::margin;
                const int     osd_height = 2 * osd_margin + font_height;

                constexpr size_t i0 = (size_t)Application::Output::OSD::Location::top_left;
                m_osd_rects[i0].left = osd_margin;
                m_osd_rects[i0].right = width - osd_margin;
                m_osd_rects[i0].top = osd_margin;
                m_osd_rects[i0].bottom = osd_height;
                m_osd_params[i0] = DT_TOP | DT_LEFT | DT_NOCLIP;

                constexpr size_t i1 = (size_t)Application::Output::OSD::Location::top_right;
                m_osd_rects[i1].left = width / 2 + osd_margin;
                m_osd_rects[i1].right = width - osd_margin;
                m_osd_rects[i1].top = osd_margin;
                m_osd_rects[i1].bottom = osd_height;
                m_osd_params[i1] = DT_TOP | DT_RIGHT | DT_NOCLIP;

                constexpr size_t i2 = (size_t)Application::Output::OSD::Location::bottom_left;
                m_osd_rects[i2].left = osd_margin;
                m_osd_rects[i2].right = width - osd_margin;
                m_osd_rects[i2].top = height - font_height - osd_margin;
                m_osd_rects[i2].bottom = height - osd_margin;
                m_osd_params[i2] = DT_TOP | DT_LEFT | DT_NOCLIP;

                constexpr size_t i3 = (size_t)Application::Output::OSD::Location::bottom_right;
                m_osd_rects[i3].left = width / 2 + osd_margin;
                m_osd_rects[i3].right = width - osd_margin;
                m_osd_rects[i3].top = height - font_height - osd_margin;
                m_osd_rects[i3].bottom = height - osd_margin;
                m_osd_params[i3] = DT_TOP | DT_RIGHT | DT_NOCLIP;

                m_input.screen.pixels_buffer_pointer
                    += m_input.screen.pixels_buffer_pitch * osd_height;
                m_input.screen.height -= osd_height * 2;
            }

            void window::draw_osd_text( [[maybe_unused]] HDC hdc )
            {
                // It's necessary to clear all text output area to erase old text
                for ( int i = 0; i < osd_locations_count; ++i )
                {
                    [[maybe_unused]] const int res = ::FillRect(
                        m_screen_buffer_dc, &m_osd_rects[i], m_window_class.hbrBackground );
                    RTL_WINAPI_CHECK( res != 0 );
                }

                // NOTE: Drawing to back buffer to prevent flickering
                HGDIOBJ object = ::SelectObject( m_screen_buffer_dc, m_osd_font );
                RTL_WINAPI_CHECK( object != nullptr );
                RTL_ASSERT( ::GetObjectType( object ) == OBJ_FONT );

                // NOTE: The same color as background one.
                ::SetBkColor( m_screen_buffer_dc, RGB( 0, 0, 0 ) );
                // ::SetBkMode( m_screen_buffer_dc, TRANSPARENT );
                ::SetTextColor( m_screen_buffer_dc, RGB( 255, 255, 255 ) );

                for ( int i = 0; i < osd_locations_count; ++i )
                {
                    [[maybe_unused]] const int res = ::DrawTextW( m_screen_buffer_dc,
                                                                  m_output.osd.text[i],
                                                                  -1,
                                                                  &m_osd_rects[i],
                                                                  m_osd_params[i] | DT_SINGLELINE );
                    RTL_WINAPI_CHECK( res != 0 );
                }

                object = ::SelectObject( m_screen_buffer_dc, object );
                RTL_ASSERT( object == m_osd_font );
            }

            void window::free_osd_text()
            {
                if ( m_osd_font )
                {
                    [[maybe_unused]] BOOL result = ::DeleteObject( m_osd_font );
                    RTL_WINAPI_CHECK( result );
                    m_osd_font = nullptr;
                }
            }
        } // namespace win

    }     // namespace impl
} // namespace rtl

    #endif
#endif