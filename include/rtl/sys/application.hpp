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

#include <rtl/int.hpp>
#include <rtl/sys/keyboard.hpp>

#if RTL_ENABLE_APP

namespace rtl
{
    class application final
    {
    public:
        struct input
        {
            void* void_source; // CAUTION: it is better to NOT touch it!!!

    #if RTL_ENABLE_APP_KEYS
            struct keys
            {
                bool state[(size_t)keyboard::keys::count];
                bool pressed[(size_t)keyboard::keys::count];
            } keys;
    #endif

    #if RTL_ENABLE_APP_CLOCK
            struct clock
            {
                // thirds per second or seconds per minute
                static constexpr auto measure = 60;

                // monotone counter of thirds (1/60 of second)
                int32_t thirds;
            } clock;
    #endif
        };

        struct output
        {
            struct screen
            {
                uint8_t* pixels;
                int      width;
                int      height;
                size_t   pitch;
            } screen;

    #if RTL_ENABLE_APP_OSD
            struct osd
            {
                static constexpr auto text_length = 256;
                static constexpr auto margin = 12;
                static constexpr auto font_size = 24;

                enum class location
                {
                    top_left,
                    top_right,
                    bottom_left,
                    bottom_right,
                    count
                };

                wchar_t text[(size_t)location::count][text_length];
            } osd;
    #endif
        };

        enum class action
        {
            none,
            close
        };

        using update_function = action( const input&, output& );

    public:
        static application& instance();

        void run( const wchar_t* app_name, update_function* update );

    private:
        application() = default;

        application( const application& ) = delete;
        application& operator=( const application& ) = delete;

        application( application&& ) = delete;
        application& operator=( application&& ) = delete;
    };
} // namespace rtl

#endif
