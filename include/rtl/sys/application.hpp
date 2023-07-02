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

#include <rtl/int.hpp>
#include <rtl/sys/keyboard.hpp>

#if RTL_ENABLE_APP

namespace rtl
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// Application.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    class Application final
    {
    public:
    #if RTL_ENABLE_APP_RESOURCES
        class Resource;
    #endif

        ////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Application environment.
        /// Hardware and OS parameters, embedded resources, etc.
        ////////////////////////////////////////////////////////////////////////////////////////////
        struct Environment
        {
    #if RTL_ENABLE_APP_AUDIO_OUTPUT
            /// @brief Display settings.
            /// Used by the audio module to calculate the audio frame buffer size.
            /// @todo: Add support for systems with multiple monitors with different frame rates.
            /// @todo: Use a more precise frame rate value.
            struct Display
            {
                /// Monitor or adapter frame rate.
                unsigned framerate;
            }
            /// Display settings.
            display;
    #endif
    #if RTL_ENABLE_APP_RESOURCES
            /// @brief Application resources.
            /// Provides access to in-app resources.
            /// @todo: Implement a resource iterator.
            struct Resources
            {
                /// Placeholder to avoid voidness of the structure.
                void* placeholder; // CAUTION: it is better NOT to touch the V̪̪̟O͇̘̞I̝̞D͇͚͜!!!

                /// @brief Opens resource.
                /// @param type Resource type.
                /// @param id Resource identifier.
                /// @return Opened resource.
                Resource open( int type, int id ) const;
            }
            /// Application resources.
            resources;
    #endif
            /// Handle to the application's main window.
            void* window_handle; // CAUTION: it is better NOT to touch the V̪̪̟O͇̘̞I̝̞D͇͚͜!!!
        };

        ////////////////////////////////////////////////////////////////////////////////////////////
        /// Application initialization parameters.
        ////////////////////////////////////////////////////////////////////////////////////////////
        struct Params
        {
    #if RTL_ENABLE_APP_AUDIO_OUTPUT
            /// Audio initialization parameters.
            struct Audio
            {
                /// Sampling frequency.
                size_t samples_per_second;
                /// The maximum latency of the input and output buffers.
                size_t max_latency_samples;
            }
            /// Audio parameters.
            audio;
    #endif
    #if RTL_ENABLE_APP_RESIZE || !RTL_ENABLE_APP_FULLSCREEN
            /// Window initialization parameters.
            struct Window
            {
                /// The client area width, in pixels.
                int width;
                /// The client area height, in pixels.
                int height;
            }
            /// Main window parameters.
            window;
    #endif
            /// Placeholder to avoid voidness of the structure.
            void* placeholder; // CAUTION: it is better NOT to touch the V̪̪̟O͇̘̞I̝̞D͇͚͜!!!
        };

        ////////////////////////////////////////////////////////////////////////////////////////////
        /// Context of input events and immutable output data.
        ////////////////////////////////////////////////////////////////////////////////////////////
        struct Input
        {
    #if RTL_ENABLE_APP_CLOCK
            /// Clock.
            struct Clock
            {
                /// Monotone counter of thirds (1/60th of second).
                /// @todo Just use frame counter?
                int32_t third_ticks;
            } clock;
    #endif

    #if RTL_ENABLE_APP_KEYS
            /// Key state.
            struct Keys
            {
                /// @brief Key state.
                /// true if the key is in the pressed state.
                bool state[keyboard::Keys::count];

                /// @brief Key pressed event.
                /// true if the key was just pressed.
                bool pressed[keyboard::Keys::count];
            }
            /// Key state.
            keys;
    #endif

    #if RTL_ENABLE_APP_AUDIO_OUTPUT
            /// Immutable audio data.
            struct Audio
            {
                /// Number of channels.
                static constexpr size_t channel_count = 2;

                /// Sampling frequency.
                size_t samples_per_second;

                /// @brief Number of samples per frame.
                /// For one channel.
                size_t samples_per_frame;

                /// Pointer to the audio output buffer.
                int16_t* output_frame_pointer;
            }
            /// Audio data.
            audio;
    #endif

            /// Immutable screen data.
            struct Screen
            {
                /// Width of the client area width of the main window in pixels.
                int width;

                /// Height of the client area width of the main window in pixels.
                int height;

    #if RTL_ENABLE_APP_SCREEN_BUFFER
                /// Pointer to the pixels buffer of the main window.
                uint8_t* pixels_buffer_pointer;
                /// Size of the pixel line in bytes.
                size_t pixels_buffer_pitch;
    #endif
            }
            /// Screen data.
            screen;

            /// Opaque handle of the main window.
            void* window_handle; // CAUTION: it is better NOT to touch the V̪̪̟O͇̘̞I̝̞D͇͚͜!!!
        };

        ////////////////////////////////////////////////////////////////////////////////////////////
        /// Mutable output data.
        ////////////////////////////////////////////////////////////////////////////////////////////
        struct Output
        {
    #if RTL_ENABLE_APP_SCREEN_BUFFER
        #if RTL_ENABLE_APP_OSD
            /// On-screen display data.
            struct OSD
            {
                /// Maximum text length including null terminating character.
                static constexpr auto text_length = 256;

                /// Margins of text from edges of the the window (in pixels).
                static constexpr auto margin = 12;

                /// Font size in logical units for a 1280 pixels wide screen.
                static constexpr auto font_size = 24;

                /// Text location.
                enum class Location
                {
                    /// In the top left corner.
                    top_left,
                    /// In the top right corner.
                    top_right,
                    /// In the bottom left corner.
                    bottom_left,
                    /// In the bottom right corner.
                    bottom_right,
                    /// Number of locations.
                    count
                };

                /// The text to display.
                wchar_t text[(size_t)Location::count][text_length];
            }
            /// On-screen display data.
            osd;
        #endif
    #endif
            /// Placeholder to avoid voidness of the structure.
            void* placeholder; // CAUTION: it is better NOT to touch the V̪̪̟O͇̘̞I̝̞D͇͚͜!!!
        };

        ////////////////////////////////////////////////////////////////////////////////////////////
        /// Application action.
        ////////////////////////////////////////////////////////////////////////////////////////////
        enum class Action
        {
            /// Do nothing.
            none,
            /// Wait for next input event.
            wait,
            /// Close application.
            close,
    #if RTL_ENABLE_APP_RESET
            /// Re-initialize the application.
            reset,
    #endif
    #if RTL_ENABLE_APP_RESIZE
            /// Switch the application window to full screen mode.
            toggle_fullscreen,
    #endif
        };

    #if RTL_ENABLE_APP_RESOURCES
        ////////////////////////////////////////////////////////////////////////////////////////////
        /// In-app resource.
        ////////////////////////////////////////////////////////////////////////////////////////////
        class Resource final
        {
        public:
            Resource() = default;
            ~Resource() = default;

            /// @brief Checking if the resource exists.
            /// @return true, if the resource exists and the data is not null.
            operator bool() const;

            /// @brief Resource size.
            /// @return Resource size in bytes.
            size_t size() const;

            /// @brief Resource data.
            /// @return Pointer to the memory location where the resource located.
            const void* data() const;

        private:
            friend struct Environment::Resources;

            Resource( void* data, size_t size );

            void*  m_data{ nullptr };
            size_t m_size{ 0 };
        };
    #endif

        ////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Setup callback.
        /// Called once before the application window will be created.
        /// @param env Application environment.
        /// @param params Parameters to set.
        /// @return true, if the application should proceed to initialization.
        /// @return false, if the application should be terminated.
        ////////////////////////////////////////////////////////////////////////////////////////////
        using setup_function = bool( const Environment& env, Params& params );

        ////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Initialization callback.
        /// Called once after the application window was created.
        /// @param env Application environment.
        /// @param input Current state of the input context.
        ////////////////////////////////////////////////////////////////////////////////////////////
        using init_function = void( const Environment& env, const Input& input );

        ////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Update callback.
        /// Called at the start of every frame or after an input event.
        /// @param input Input context.
        /// @param output Output context.
        /// @return The action that the application should take.
        ////////////////////////////////////////////////////////////////////////////////////////////
        using update_function = Action( const Input& input, Output& output );

        ////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Termination callback.
        /// Called once when the application closes.
        ////////////////////////////////////////////////////////////////////////////////////////////
        using terminate_function = void();

    public:
        /// @brief Application singleton accessor.
        /// @return Current instance of application.
        static Application& instance();

        /// @brief Run application.
        /// @param app_name Application name (displayed in the titlebar of the main window).
        /// @param on_setup Setup callback function. May be nullptr.
        /// @param on_init Init callback function. May be nullptr.
        /// @param on_update Update callback function. May be nullptr.
        /// @param on_terminate Terminate callback function. May be nullptr.
        void run( const wchar_t*      app_name,
                  setup_function*     on_setup,
                  init_function*      on_init,
                  update_function*    on_update,
                  terminate_function* on_terminate );

    private:
        Application() = default;

        Application( const Application& ) = delete;
        Application& operator=( const Application& ) = delete;

        Application( Application&& ) = delete;
        Application& operator=( Application&& ) = delete;
    };
} // namespace rtl

#endif
