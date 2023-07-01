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

#if RTL_ENABLE_CHRONO_CLOCK

    #include <rtl/chrono.hpp>

    #include "win.hpp"

namespace rtl
{
    namespace impl
    {
        class performance_counter final
        {
        public:
            void init()
            {
                [[maybe_unused]] BOOL result = ::QueryPerformanceFrequency( &m_frequency );
                RTL_WINAPI_CHECK( result );
            }

            int64_t count() const
            {
                LARGE_INTEGER counter;

                [[maybe_unused]] BOOL result = ::QueryPerformanceCounter( &counter );
                RTL_WINAPI_CHECK( result );

                return counter.QuadPart;
            }

            int64_t frequency() const
            {
                return m_frequency.QuadPart;
            }

        private:
            LARGE_INTEGER m_frequency;
        } g_performance_counter;
    } // namespace impl

    namespace chrono
    {
        steady_clock::time_point steady_clock::now()
        {
            return steady_clock::time_point( microseconds(
                static_cast<int>( rtl::impl::g_performance_counter.count() * 1000000ll
                                  / rtl::impl::g_performance_counter.frequency() ) ) );
        }
    } // namespace chrono
} // namespace rtl

#endif
