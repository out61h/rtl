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
    #if RTL_ENABLE_APP_OSD

namespace rtl
{
    namespace impl
    {
        namespace win
        {
            void window::init_osd_text( int width, int height )
            {
                RTL_ASSERT( m_osd_font == nullptr );

                m_osd_font = ::CreateFontW( application::output::osd::font_size * width / 1280,
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
                HGDIOBJ object = ::SelectObject( m_device_context_handle, m_osd_font );
                RTL_WINAPI_CHECK( object != nullptr );
                RTL_ASSERT( ::GetObjectType( object ) == OBJ_FONT );

                TEXTMETRIC tm;

                [[maybe_unused]] BOOL result = ::GetTextMetricsW( m_device_context_handle, &tm );
                RTL_WINAPI_CHECK( result );

                object = ::SelectObject( m_device_context_handle, object );
                RTL_ASSERT( object == m_osd_font );

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

        #if RTL_ENABLE_APP_SCREEN
                m_output.screen.pixels += m_output.screen.pitch * osd_height;
                m_output.screen.height -= osd_height * 2;
        #endif
            }

            void window::draw_osd_text()
            {
                for ( int i = 0; i < osd_locations_count; ++i )
                {
                    [[maybe_unused]] const int res = ::FillRect(
                        m_device_context_handle, &m_osd_rects[i], m_window_class.hbrBackground );
                    RTL_WINAPI_CHECK( res != 0 );
                }

                HGDIOBJ object = ::SelectObject( m_device_context_handle, m_osd_font );
                RTL_WINAPI_CHECK( object != nullptr );
                RTL_ASSERT( ::GetObjectType( object ) == OBJ_FONT );

                // NOTE: The same color as background one.
                ::SetBkColor( m_device_context_handle, RGB( 0, 0, 0 ) );
                // ::SetBkMode( m_device_context_handle, TRANSPARENT );
                ::SetTextColor( m_device_context_handle, RGB( 255, 255, 255 ) );

                for ( int i = 0; i < osd_locations_count; ++i )
                {
                    [[maybe_unused]] const int res = ::DrawTextW( m_device_context_handle,
                                                                  m_output.osd.text[i],
                                                                  -1,
                                                                  &m_osd_rects[i],
                                                                  m_osd_params[i] | DT_SINGLELINE );
                    RTL_WINAPI_CHECK( res != 0 );
                }

                object = ::SelectObject( m_device_context_handle, object );
                RTL_ASSERT( object == m_osd_font );
            }
        } // namespace win
    }     // namespace impl
} // namespace rtl
    #endif
#endif