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
    // TODO: Remove padding placeholders of void* type
    class application final
    {
    public:
    #if RTL_ENABLE_APP_RESOURCES
        class resource;
    #endif

        struct environment
        {
            void* window_handle; // CAUTION: it is better to NOT touch the V̪̪̟O͇̘̞I̝̞D͇͚͜!!!

    #if RTL_ENABLE_APP_AUDIO_OUTPUT 
            // NOTE: Used by audio module to calculate the size of the audio frame buffer
            // TODO: Add support of multimonitor systems with different framerates
            struct display
            {
                unsigned framerate;
            } display;
    #endif

    #if RTL_ENABLE_APP_RESOURCES
            struct resources
            {
                void* void_resource; // CAUTION: it is better to NOT touch the V̪̪̟O͇̘̞I̝̞D͇͚͜!!!

                resource open( int type, int id ) const;

                // TODO: Implement resource iterator
            } resources;
    #endif
        };

        struct params
        {
            void* void_param; // CAUTION: it is better to NOT touch the V̪̪̟O͇̘̞I̝̞D͇͚͜!!!

    #if RTL_ENABLE_APP_AUDIO_OUTPUT
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
    #if RTL_ENABLE_APP_CLOCK
            struct clock
            {
                // monotone counter of thirds (1/60 of second)
                int32_t third_ticks;
            } clock;
    #endif

    #if RTL_ENABLE_APP_KEYS
            struct keys
            {
                bool state[keyboard::keys::count];
                bool pressed[keyboard::keys::count];
            } keys;
    #endif

    #if RTL_ENABLE_APP_AUDIO_OUTPUT
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

            void* window_handle; // CAUTION: it is better to NOT touch the V̪̪̟O͇̘̞I̝̞D͇͚͜!!!
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
    #if RTL_ENABLE_APP_RESET
            reset,
    #endif
    #if RTL_ENABLE_APP_RESIZE
            toggle_fullscreen,
    #endif
        };

    #if RTL_ENABLE_APP_RESOURCES
        class resource final
        {
        public:
            resource() = default;
            ~resource() = default;

            operator bool() const;

            size_t      size() const;
            const void* data() const;

        private:
            friend struct environment::resources;

            resource( void* data, size_t size );

            void*  m_data{ nullptr };
            size_t m_size{ 0 };
        };
    #endif

        using setup_function = bool( const environment&, params& );
        using init_function = void( const environment&, const input& );
        using update_function = action( const input&, output& );
        using terminate_function = void();

    public:
        static application& instance();

        void run( const wchar_t*      app_name,
                  setup_function*     on_setup,
                  init_function*      on_init,
                  update_function*    on_update,
                  terminate_function* on_terminate );

    private:
        application() = default;

        application( const application& ) = delete;
        application& operator=( const application& ) = delete;

        application( application&& ) = delete;
        application& operator=( application&& ) = delete;
    };
} // namespace rtl

#endif
