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

#if RTL_ENABLE_APP
    #if RTL_ENABLE_APP_AUDIO_OUTPUT

        #include <rtl/int.hpp>
        #include <rtl/sys/impl/win.hpp>
        #include <rtl/vector.hpp>

namespace rtl
{
    namespace impl
    {
        namespace win
        {
            class audio final
            {
            public:
                audio( unsigned samples_per_second,
                       unsigned samples_per_frame,
                       unsigned frames_per_buffer );
                ~audio();

                [[nodiscard]] int16_t* start();
                [[nodiscard]] int16_t* commit();

                void stop();

            private:
                static void CALLBACK
                    wave_out_proc( HWAVEOUT, UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR );

                WAVEFORMATEX         m_wave_format{ 0 };
                bool                 m_started{ false };
                bool                 m_pad{ false };
                HWAVEOUT             m_wave_out{ nullptr };
                rtl::vector<WAVEHDR> m_wave_headers;
                rtl::vector<int16_t> m_buffer;
                size_t               m_write_index{ 0 };
            };
        } // namespace win

    }     // namespace impl
} // namespace rtl

    #endif
#endif
