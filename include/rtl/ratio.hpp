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
#include <rtl/numeric.hpp>

namespace rtl
{
    namespace impl
    {
        using ratio_type = int32_t;
    }

    template<impl::ratio_type Num, impl::ratio_type Den = 1>
    class ratio final
    {
    public:
        using type = ratio<Num, Den>;

        static constexpr impl::ratio_type num = sign( Den ) * Num / gcd( Num, Den );
        static constexpr impl::ratio_type den = abs( Den ) / gcd( Num, Den );
    };

    using nano = ratio<1, 1000000000>;
    using micro = ratio<1, 1000000>;
    using milli = ratio<1, 1000>;
    using kilo = ratio<1, 1000>;
    using mega = ratio<1, 1000000>;
    using giga = ratio<1, 1000000000>;

    template<typename R1, typename R2>
    using ratio_divide = ratio<R1::num * R2::den, R1::den * R2::num>;
} // namespace rtl
