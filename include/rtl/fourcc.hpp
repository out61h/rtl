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

#ifndef _MSC_VER
    #error Only MSVC compiler is supported
#endif

#ifndef _M_IX86
    #error Unsupported CPU architecture
#endif

namespace rtl
{
    [[nodiscard]] constexpr uint32_t make_fourcc( uint8_t a, uint8_t b, uint8_t c, uint8_t d )
    {
        return static_cast<uint32_t>( ( d << 24u ) | ( c << 16u ) | ( b << 8u ) | a );
    }
} // namespace rtl
