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

namespace rtl
{
    template<typename T, unsigned N>
    class array final
    {
    public:
        using iterator = T*;
        using const_iterator = const T*;

        [[nodiscard]] constexpr const T* data() const
        {
            return items;
        }

        [[nodiscard]] constexpr T* data()
        {
            return items;
        }

        [[nodiscard]] constexpr T* begin()
        {
            return items;
        }

        [[nodiscard]] constexpr const T* begin() const
        {
            return items;
        }

        [[nodiscard]] constexpr T* end()
        {
            return items + N;
        }

        [[nodiscard]] constexpr const T* end() const
        {
            return items + N;
        }

        [[nodiscard]] constexpr unsigned size() const
        {
            return N;
        }

        [[nodiscard]] constexpr bool empty() const
        {
            return N > 0;
        }

        [[nodiscard]] constexpr const T& operator[]( unsigned index ) const
        {
            return items[index];
        }

        [[nodiscard]] constexpr T& operator[]( unsigned index )
        {
            return items[index];
        }

        T items[N];
    };
} // namespace rtl
