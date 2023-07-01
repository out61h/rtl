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
        [[nodiscard]] static constexpr int min()
        {
            return -2147483647 - 1;
        }

        [[nodiscard]] static constexpr int max()
        {
            return 2147483647;
        }

        static constexpr bool is_signed = true;
    };

    template<>
    struct numeric_limits<unsigned>
    {
        [[nodiscard]] static constexpr unsigned min()
        {
            return 0;
        }

        [[nodiscard]] static constexpr unsigned max()
        {
            return 4294967295;
        }

        static constexpr bool is_signed = false;
    };

    template<>
    struct numeric_limits<unsigned long>
    {
        [[nodiscard]] static constexpr unsigned min()
        {
            return 0;
        }

        [[nodiscard]] static constexpr unsigned max()
        {
            return 4294967295;
        }

        static constexpr bool is_signed = false;
    };

    template<>
    struct numeric_limits<int16_t>
    {
        [[nodiscard]] static constexpr int16_t min()
        {
            return -32768;
        }

        [[nodiscard]] static constexpr int16_t max()
        {
            return 32767;
        }

        static constexpr bool is_signed = true;
    };

    template<>
    struct numeric_limits<int64_t>
    {
        [[nodiscard]] static constexpr int64_t min()
        {
            return -9223372036854775807 - 1;
        }

        [[nodiscard]] static constexpr int64_t max()
        {
            return 9223372036854775807;
        }

        static constexpr bool is_signed = true;
    };
} // namespace rtl
