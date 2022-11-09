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
    namespace allocators
    {
        template<typename T, int Size>
        class grow_only final
        {
        public:
            using value_type = T;

            constexpr grow_only() = default;

            [[nodiscard]] constexpr T* allocate( size_t n )
            {
                if ( n > Size - m_size )
                    return nullptr;

                const size_t offset = m_size;
                m_size += n;
                return m_buffer + offset;
            }

            constexpr void reset()
            {
                m_size = 0;
            }

        private:
            T      m_buffer[Size];
            size_t m_size;
        };
    } // namespace allocators

} // namespace rtl
