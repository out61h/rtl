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
#include <rtl/sys/impl.hpp>

#include <rtl/random.hpp>
#include <rtl/sys/application.hpp>

using rtl::Application;
using rtl::random;
using namespace rtl::keyboard;

static random<4096> g_random;

void main()
{
    Application::instance().run(
        L"N O I S E",
        []( const Application::Environment&, Application::Params& params )
        {
            params.window = { 640, 480 };
            params.audio = { 48000, 24000 };
            return true;
        },
        []( const Application::Environment&, [[maybe_unused]] const Application::Input& input )
        {
            g_random.init( 0x1337c0de );
        },
        []( [[maybe_unused]] const Application::Input& input,
            [[maybe_unused]] Application::Output&      output )
        {
            if ( input.keys.pressed[Keys::escape] )
                return Application::Action::close;

            auto* line = input.screen.pixels_buffer_pointer;

            for ( int i = 0; i < input.screen.height; ++i )
            {
                auto* pixel = line;

                for ( int k = 0; k < input.screen.width; ++k )
                {
                    const rtl::uint8_t pix = g_random.rand() & 0xff;

                    *pixel++ = pix;
                    *pixel++ = pix;
                    *pixel++ = pix;
                }

                line += input.screen.pixels_buffer_pitch;
            }

            auto* samples = input.audio.output_frame_pointer;

            for ( rtl::size_t i = 0; i < input.audio.samples_per_frame * input.audio.channel_count;
                  ++i )
            {
                *samples++ = static_cast<rtl::int16_t>( g_random.rand() ) >> 8;
            }

            return Application::Action::wait;
        },
        nullptr );
}
