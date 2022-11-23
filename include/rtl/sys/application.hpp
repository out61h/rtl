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
        struct params
        {
            void* void_param; // CAUTION: it is better to NOT touch the V̪̪̟O͇̘̞I̝̞D͇͚͜!!!

    #if RTL_ENABLE_APP_AUDIO
            struct audio
            {
                size_t samples_per_second;
                size_t max_latency_samples;
            } audio;
    #endif

    #if RTL_ENABLE_APP_RESIZE || !RTL_ENABLE_APP_FULLSCREEN
            struct window
            {
                int width;
                int height;
            } window;
    #endif
        };

        struct input
        {
            void* void_source; // CAUTION: it is better to NOT touch the V̪̪̟O͇̘̞I̝̞D͇͚͜!!!

    #if RTL_ENABLE_APP_KEYS
            struct keys
            {
                bool state[keyboard::keys::count];
                bool pressed[keyboard::keys::count];
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

    #if RTL_ENABLE_APP_AUDIO
            struct audio
            {
                static constexpr size_t channel_count = 2;

                size_t samples_per_second;
                size_t samples_per_frame;

                int16_t* frame;
            } audio;
    #endif

            struct screen
            {
                int width;
                int height;

    #if RTL_ENABLE_APP_SCREEN_BUFFER
                uint8_t* pixels;
                size_t   pitch;
    #endif
            } screen;
        };

        struct output
        {
            void* void_sink; // CAUTION: it is better to NOT touch the V̪̪̟O͇̘̞I̝̞D͇͚͜!!!

    #if RTL_ENABLE_APP_SCREEN_BUFFER
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
    #endif
        };

        enum class action
        {
            none,
            close,
    #if RTL_ENABLE_APP_RESIZE
            toggle_fullscreen,
    #endif
        };

        using reset_function = void( const input& );
        using update_function = action( const input&, output& );

    public:
        static application& instance();

        void run( const wchar_t*   app_name,
                  const params&    app_params,
                  reset_function*  on_reset,
                  update_function* on_update );

    private:
        application() = default;

        application( const application& ) = delete;
        application& operator=( const application& ) = delete;

        application( application&& ) = delete;
        application& operator=( application&& ) = delete;
    };
} // namespace rtl

#endif
