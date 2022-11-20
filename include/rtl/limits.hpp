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

namespace rtl
{
    template<typename T>
    struct numeric_limits
    {
    };

    // TODO: add more specializations
    template<>
    struct numeric_limits<int>
    {
        static constexpr int min()
        {
            return -2147483647 - 1;
        }

        static constexpr int max()
        {
            return 2147483647;
        }

        static constexpr bool is_signed = true;
    };

    template<>
    struct numeric_limits<unsigned>
    {
        static constexpr unsigned min()
        {
            return 0;
        }

        static constexpr unsigned max()
        {
            return 4294967295;
        }

        static constexpr bool is_signed = false;
    };

    template<>
    struct numeric_limits<int16_t>
    {
        static constexpr int16_t min()
        {
            return -32768;
        }

        static constexpr int16_t max()
        {
            return 32767;
        }

        static constexpr bool is_signed = true;
    };
} // namespace rtl
