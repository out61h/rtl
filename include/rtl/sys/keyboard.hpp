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
    namespace keyboard
    {
        namespace keys
        {
            // TODO: define and handle more keys
            enum
            {
                // NOTE: Virtual key codes from Win (as other platforms aren't supported)
                any,
                enter = 0x0D,
                control = 0x11,
                escape = 0x1B,
                space = 0x20,
                f1 = 0x70,
                f2,
                f3,
                f4,
                f5,
                f6,
                f7,
                f8,
                f9,
                f10,
                f11,
                f12,
                count = 256
            };
        } // namespace keys

    } // namespace keyboard
} // namespace rtl
