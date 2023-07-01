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
#include <rtl/limits.hpp>
#include <rtl/numeric.hpp>
#include <rtl/ratio.hpp>
#include <rtl/type_traits.hpp>

namespace rtl
{
    namespace chrono
    {
        template<typename Rep, typename Period = rtl::ratio<1>>
        class duration final
        {
        public:
            using period = Period;
            using rep = Rep;

            constexpr static duration zero()
            {
                return duration();
            }

            constexpr duration() = default;
            constexpr duration( const duration& ) = default;
            // cppcheck-suppress operatorEq
            constexpr duration& operator=( const duration& other ) = default;

            template<class Rep2, class Period2>
            constexpr duration( const duration<Rep2, Period2>& other )
            {
                // TODO: Generalize implementation to floating point \Rep
                static_assert( is_floating_point<Rep2>::value == false_type::value );

                constexpr intmax_t div = Period2::den * Period::num;
                constexpr intmax_t num = Period2::num * Period::den;
                constexpr intmax_t gcd = rtl::gcd( div, num );
                constexpr intmax_t num_gcd = num / gcd;
                constexpr intmax_t div_gcd = div / gcd;

                // TODO: Detect possible overflow (at compile time?)
                m_value = other.count() * num_gcd / div_gcd;
            }

            constexpr explicit duration( rep value )
                : m_value( value )
            {
            }

            [[nodiscard]] constexpr rep count() const
            {
                return m_value;
            }

        private:
            rep m_value{ 0 };
        };

        // specialization of \common_type for \duration
        template<typename Rep1, typename Period1, typename Rep2, typename Period2>
        struct common_type<duration<Rep1, Period1>, duration<Rep2, Period2>>
        {
            using type = chrono::duration<
                typename common_type<Rep1, Rep2>::type,
                ratio<gcd( Period1::num, Period2::num ), lcm( Period1::den, Period2::den )>>;
        };

        template<typename Rep1, typename Period1, typename Rep2, typename Period2>
        [[nodiscard]] constexpr bool operator>=( const duration<Rep1, Period1>& lhs,
                                                 const duration<Rep2, Period2>& rhs )
        {
            using type = common_type<duration<Rep1, Period1>, duration<Rep2, Period2>>::type;

            const type lhs_v( lhs );
            const type rhs_v( rhs );

            return lhs_v.count() >= rhs_v.count();
        }

        template<typename Rep1, typename Period1, typename Rep2, typename Period2>
        [[nodiscard]] constexpr
            typename common_type<duration<Rep1, Period1>, duration<Rep2, Period2>>::type
            operator+( const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs )
        {
            using type = common_type<duration<Rep1, Period1>, duration<Rep2, Period2>>::type;

            const type lhs_v( lhs );
            const type rhs_v( rhs );

            return type( lhs_v.count() + rhs_v.count() );
        }

        template<typename Rep1, typename Period1, typename Rep2, typename Period2>
        [[nodiscard]] constexpr
            typename common_type<duration<Rep1, Period1>, duration<Rep2, Period2>>::type
            operator-( const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs )
        {
            using type = common_type<duration<Rep1, Period1>, duration<Rep2, Period2>>::type;

            const type lhs_v( lhs );
            const type rhs_v( rhs );

            return type( lhs_v.count() - rhs_v.count() );
        }

        namespace impl
        {
            using duration_rep_type = int32_t;
        }

        using nanoseconds = duration<impl::duration_rep_type, nano>;
        using microseconds = duration<impl::duration_rep_type, micro>;
        using milliseconds = duration<impl::duration_rep_type, milli>;
        using thirds = duration<impl::duration_rep_type, ratio<1, 60>>;
        using seconds = duration<impl::duration_rep_type>;
        using minutes = duration<impl::duration_rep_type, ratio<60>>;
        using hours = duration<impl::duration_rep_type, ratio<3600>>;
        using days = duration<impl::duration_rep_type, ratio<86400>>;
        using weeks = duration<impl::duration_rep_type, ratio<604800>>;

        template<typename Clock, typename Duration = Clock::duration>
        class time_point final
        {
        public:
            using clock = Clock;
            using duration = Duration;
            using period = typename Duration::period;
            using rep = typename Duration::rep;

            constexpr time_point() = default;

            constexpr explicit time_point( const duration& d )
                : m_duration( d )
            {
            }

            template<typename Duration2>
            constexpr time_point( const time_point<Clock, Duration2>& t )
                : m_duration( t.m_duration )
            {
            }

            constexpr duration time_since_epoch() const
            {
                return m_duration;
            }

        private:
            duration m_duration;
        };

        template<typename Clock, typename Duration1, typename Rep2, typename Period2>
        constexpr time_point<Clock, common_type_t<Duration1, duration<Rep2, Period2>>>
        operator+( const time_point<Clock, Duration1>& pt, const duration<Rep2, Period2>& d );

        template<typename Rep1, typename Period1, typename Clock, typename Duration2>
        constexpr time_point<Clock, common_type_t<duration<Rep1, Period1>, Duration2>>
        operator+( const duration<Rep1, Period1>& d, const time_point<Clock, Duration2>& pt );

        template<typename Clock, typename Duration1, typename Rep2, typename Period2>
        constexpr time_point<Clock, common_type_t<Duration1, duration<Rep2, Period2>>>
        operator-( const time_point<Clock, Duration1>& pt, const duration<Rep2, Period2>& d );

        template<typename Clock, typename Duration1, typename Duration2>
        constexpr common_type_t<Duration1, Duration2>
        operator-( const time_point<Clock, Duration1>& lhs,
                   const time_point<Clock, Duration2>& rhs )
        {
            using type = common_type_t<Duration1, Duration2>;

            const type lhs_v( lhs.time_since_epoch() );
            const type rhs_v( rhs.time_since_epoch() );

            return type( lhs_v.count() - rhs_v.count() );
        }

#if RTL_ENABLE_CHRONO_CLOCK
        class steady_clock final
        {
        public:
            using duration = microseconds;
            using period = duration::period;
            using rep = duration::rep;
            using time_point = time_point<steady_clock>;

            constexpr static bool is_steady = true;

            static time_point now();
        };
#endif

    } // namespace chrono
} // namespace rtl
