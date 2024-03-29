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
    #if RTL_ENABLE_APP_AUDIO_OUTPUT
        #if RTL_ENABLE_RUNTIME_CHECKS
            #define RTL_MM_WAVEOUT_CHECK( code ) \
                rtl::impl::win::check_mm_waveout( code, __FILE__, __LINE__ )
        #else
            #define RTL_MM_WAVEOUT_CHECK( condition )
        #endif

namespace rtl
{
    namespace impl
    {
        namespace win
        {
            void window::create_audio()
            {
                RTL_ASSERT( !m_audio );

                m_input.audio.samples_per_second = m_params.audio.samples_per_second;
                m_input.audio.samples_per_frame
                    = m_params.audio.samples_per_second / m_environment.display.framerate;

                if ( m_input.audio.samples_per_second > 0 )
                {
                    const size_t buffers_count
                        = m_params.audio.max_latency_samples / m_input.audio.samples_per_frame;

                    m_audio = new audio( m_input.audio.samples_per_second,
                                         m_input.audio.samples_per_frame,
                                         buffers_count > 1 ? buffers_count : 2 );
                }

                restart_audio();
            }

            void window::destroy_audio()
            {
                delete m_audio;
                m_audio = nullptr;
                m_input.audio.output_frame_pointer = nullptr;
            }

            void window::commit_audio()
            {
                if ( m_audio )
                    m_input.audio.output_frame_pointer = m_audio->commit();
            }

            void window::restart_audio()
            {
                if ( m_audio )
                    m_input.audio.output_frame_pointer = m_audio->start();
                else
                    m_input.audio.output_frame_pointer = nullptr;
            }

        #if RTL_ENABLE_RUNTIME_CHECKS
            void check_mm_waveout( DWORD code, const char* file, int line )
            {
                if ( code == MMSYSERR_NOERROR )
                    return;

                constexpr int length = MAXERRORLENGTH;
                char          str[length];
                ::waveOutGetErrorTextA( code, str, length );

                assert( false, (int)code, str, file, line );
            }
        #endif

            audio::audio( unsigned samples_per_second,
                          unsigned samples_per_frame,
                          unsigned frames_per_buffer )
            {
                RTL_ASSERT( samples_per_second > 0 );
                RTL_ASSERT( samples_per_frame > 0 );
                RTL_ASSERT( frames_per_buffer > 1 );

                constexpr size_t channels_count = Application::Input::Audio::channel_count;

                m_wave_format.wFormatTag = WAVE_FORMAT_PCM;
                m_wave_format.nChannels = channels_count;
                m_wave_format.cbSize = 0;
                m_wave_format.nSamplesPerSec = static_cast<DWORD>( samples_per_second );
                m_wave_format.wBitsPerSample = 16;
                m_wave_format.nBlockAlign
                    = m_wave_format.wBitsPerSample * m_wave_format.nChannels / 8u;
                m_wave_format.nAvgBytesPerSec
                    = m_wave_format.nSamplesPerSec * m_wave_format.nBlockAlign;

                MMRESULT result = ::waveOutOpen( &m_wave_out,
                                                 WAVE_MAPPER,
                                                 &m_wave_format,
                                                 reinterpret_cast<DWORD_PTR>( wave_out_proc ),
                                                 reinterpret_cast<DWORD_PTR>( this ),
                                                 CALLBACK_FUNCTION );
                RTL_MM_WAVEOUT_CHECK( result );

                m_wave_headers.resize( frames_per_buffer );
                const unsigned block_size = m_wave_format.nChannels * samples_per_frame;
                m_buffer.resize( block_size * frames_per_buffer );

                for ( size_t i = 0; i < m_wave_headers.size(); ++i )
                {
                    WAVEHDR& header = m_wave_headers[i];
                    memset( &header, 0, sizeof( header ) );

                    header.lpData = reinterpret_cast<LPSTR>( m_buffer.data() + block_size * i );
                    header.dwBufferLength = block_size * sizeof( int16_t );
                    header.dwUser = i;

                    result = ::waveOutPrepareHeader( m_wave_out, &header, sizeof( WAVEHDR ) );
                    RTL_MM_WAVEOUT_CHECK( result );
                }

                result = ::waveOutReset( m_wave_out );
                RTL_MM_WAVEOUT_CHECK( result );
            }

            audio::~audio()
            {
                if ( m_wave_out )
                {
                    MMRESULT result = ::waveOutReset( m_wave_out );
                    RTL_MM_WAVEOUT_CHECK( result );

                    for ( auto& header : m_wave_headers )
                    {
                        result = ::waveOutUnprepareHeader( m_wave_out, &header, sizeof( WAVEHDR ) );
                        RTL_MM_WAVEOUT_CHECK( result );
                    }

                    result = ::waveOutClose( m_wave_out );
                    RTL_MM_WAVEOUT_CHECK( result );
                }
            }

            void audio::wave_out_proc( HWAVEOUT /* hwo */,
                                       UINT      msg,
                                       DWORD_PTR instance,
                                       DWORD_PTR param1,
                                       DWORD_PTR /* param2 */ )
            {
                [[maybe_unused]] window* const that = reinterpret_cast<window*>( instance );

                switch ( msg )
                {
                case WOM_DONE:
                {
                    // XXX: also whern reset

                    // RTL_APP_AUDIO_STATS
                    // XXX: sample buffers counts
                    // XXX: detect if no buffers left == underrun

                    // XXX: perform device restart
                    [[maybe_unused]] WAVEHDR* header = reinterpret_cast<WAVEHDR*>( param1 );
                    RTL_ASSERT( header->dwFlags & WHDR_DONE );
                    break;
                }
                }
            }

            [[nodiscard]] int16_t* audio::start()
            {
                [[maybe_unused]] MMRESULT result = ::waveOutReset( m_wave_out );
                RTL_MM_WAVEOUT_CHECK( result );

                m_write_index = 0;
                m_started = false;

                return reinterpret_cast<int16_t*>( m_wave_headers.front().lpData );
            }

            void audio::stop()
            {
                [[maybe_unused]] MMRESULT result = ::waveOutReset( m_wave_out );
                RTL_MM_WAVEOUT_CHECK( result );

                m_write_index = 0;
                m_started = false;
            }

            int16_t* audio::commit()
            {
                MMRESULT result = ::waveOutWrite(
                    m_wave_out, &m_wave_headers[m_write_index], sizeof( WAVEHDR ) );
                RTL_MM_WAVEOUT_CHECK( result );

                m_write_index = ( m_write_index + 1 ) % m_wave_headers.size();

                WAVEHDR* header = &m_wave_headers[m_write_index];

                if ( !m_started )
                {
                    if ( m_write_index == m_wave_headers.size() - 1 )
                    {
                        result = ::waveOutRestart( m_wave_out );
                        RTL_MM_WAVEOUT_CHECK( result );

                        m_started = true;
                    }
                }
                else
                {
                    // XXX: log wait time (SUMM, N)

                    // XXX: timeout with frame duration and detect buffer overrun after that?
                    for ( ;; )
                    {
                        // XXX: really need this?
                        // 20-90 CPU cycles
                        ::MemoryBarrier();

                        if ( header->dwFlags & WHDR_DONE )
                            break;

                        // XXX: ????
                        ::_mm_pause();
                    }
                }

                return reinterpret_cast<int16_t*>( header->lpData );
            }

        } // namespace win

    }     // namespace impl
} // namespace rtl

    #endif
#endif
