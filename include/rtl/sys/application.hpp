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

#if RTL_ENABLE_APPLICATION

namespace rtl
{
    class application final
    {
    public:
        struct input
        {
            // cppcheck-suppress unusedStructMember
            void* void_source; // CAUTION: it is better to NOT touch it!!!

    #if RTL_ENABLE_APPLICATION_KEYS
            struct keys
            {
                // cppcheck-suppress unusedStructMember
                bool state[(size_t)keyboard::keys::count];
                // cppcheck-suppress unusedStructMember
                bool pressed[(size_t)keyboard::keys::count];
            } keys;
    #endif

    #if RTL_ENABLE_APPLICATION_CLOCK
            struct clock
            {
                // cppcheck-suppress unusedStructMember
                static constexpr auto measure = 60;
                // cppcheck-suppress unusedStructMember
                int32_t thirds;
            } clock;
    #endif
        };

        struct output
        {
            struct screen
            {
                // cppcheck-suppress unusedStructMember
                uint8_t* pixels;
                // cppcheck-suppress unusedStructMember
                int width;
                // cppcheck-suppress unusedStructMember
                int height;
                // cppcheck-suppress unusedStructMember
                size_t pitch;
            } screen;

    #if RTL_ENABLE_APPLICATION_OSD
            struct osd
            {
                // cppcheck-suppress unusedStructMember
                static constexpr auto text_length = 256;

                enum class location
                {
                    top_left,
                    top_right,
                    bottom_left,
                    bottom_right,
                    count
                };

                // cppcheck-suppress unusedStructMember
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
