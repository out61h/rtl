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
#include <rtl/sys/impl.hpp>

#include <rtl/random.hpp>
#include <rtl/sys/application.hpp>

using rtl::application;
using rtl::random;
using namespace rtl::keyboard;

static random<4096> g_random;

void main()
{
    application::instance().run(
        L"noise",
        []( const application::environment&, application::params& params )
        {
            params.window = { 640, 480 };
            return true;
        },
        []( const application::environment&, [[maybe_unused]] const application::input& input )
        {
            g_random.init( 0x1337c0de );
        },
        []( [[maybe_unused]] const application::input& input,
            [[maybe_unused]] application::output&      output )
        {
            if ( input.keys.pressed[keys::escape] )
                return application::action::close;

            auto* line = input.screen.pixels;

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

                line += input.screen.pitch;
            }

            return application::action::none;
        },
        nullptr );
}
