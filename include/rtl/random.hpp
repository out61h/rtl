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
    // TODO: implement STL-like API

    // Complementary-multiply-with-carry (CMWC) pseudorandom number generator.
    // https://en.wikipedia.org/wiki/Multiply-with-carry_pseudorandom_number_generator
    template<int cycle_length>
    class random final
    {
    public:
        static_assert( cycle_length >= 4 );

        constexpr void init( uint32_t seed )
        {
            carry = 362436;
            index = cycle_length - 1;

            constexpr uint32_t phi = 0x9e3779b9;

            queue[0] = seed;
            queue[1] = queue[0] + phi;
            queue[2] = queue[1] + phi;

            for ( int i = 3; i < cycle_length; i++ )
                queue[i] = queue[i - 3] ^ queue[i - 2] ^ phi ^ i;
        }

        [[nodiscard]] constexpr uint32_t rand()
        {
            constexpr uint64_t a = 18782LL;
            constexpr uint32_t r = 0xfffffffe;

            index = ( index + 1 ) & ( cycle_length - 1 );

            const uint64_t t = a * queue[index] + carry;

            carry = static_cast<uint32_t>( t >> 32ull );
            uint32_t x = static_cast<uint32_t>( t ) + carry;

            if ( x < carry )
            {
                x++;
                carry++;
            }

            return ( queue[index] = r - x );
        }

        uint32_t queue[cycle_length];
        uint32_t carry;
        uint32_t index;
    };

} // namespace rtl
