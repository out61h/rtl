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
        ////////////////////////////////////////////////////////////////////////////////////////////
        /// Key codes.
        ////////////////////////////////////////////////////////////////////////////////////////////
        struct Keys
        {
            /// @todo Define and handle more keys.
            /// @note Virtual key codes from Win (as other platforms aren't supported)
            enum
            {
                /// Definitely any key.
                any,
                /// Enter.
                enter = 0x0D,
                /// Ctrl.
                control = 0x11,
                /// ESC.
                escape = 0x1B,
                /// Space.
                space = 0x20,
                /// F1.
                f1 = 0x70,
                /// F2.
                f2,
                /// F3.
                f3,
                /// F4.
                f4,
                /// F5.
                f5,
                /// F6.
                f6,
                /// F7.
                f7,
                /// F8.
                f8,
                /// F9.
                f9,
                /// F10.
                f10,
                /// F11.
                f11,
                /// F12.
                f12,
                /// Number of keys.
                count = 256
            };
        };

    } // namespace keyboard
} // namespace rtl
