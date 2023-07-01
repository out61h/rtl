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
#include <rtl/sys/impl/win.hpp>

#if RTL_ENABLE_APP
namespace rtl
{
    namespace impl
    {
        namespace win
        {
            void window::init_environment()
            {
    #if RTL_ENABLE_APP_AUDIO_OUTPUT
                DEVMODEA mode;

                [[maybe_unused]] BOOL result
                    = ::EnumDisplaySettingsA( nullptr, ENUM_CURRENT_SETTINGS, &mode );
                RTL_WINAPI_CHECK( result );
                RTL_ASSERT( mode.dmDisplayFrequency > 1 );

                m_environment.display.framerate = mode.dmDisplayFrequency;
                m_environment.window_handle = m_window_handle;
    #endif
            }

        } // namespace win
    }     // namespace impl
} // namespace rtl

#endif
