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

#ifndef RTL_IMPLEMENTATION
    #error "Do not include implementation header directly, use <rtl/sys/impl.hpp>"
#endif

#include <rtl/sys/keyboard.hpp>

#include "win.hpp"

namespace rtl
{
    namespace impl
    {
        namespace win
        {
            keyboard::keys virtual_key_to_enum( WPARAM wKey )
            {
                switch ( wKey )
                {
                case VK_ESCAPE:
                    return keyboard::keys::esc;

                case VK_RETURN:
                    return keyboard::keys::enter;

                case VK_SPACE:
                    return keyboard::keys::space;

                case VK_F1:
                    return keyboard::keys::f1;

                case VK_F2:
                    return keyboard::keys::f2;

                case VK_F3:
                    return keyboard::keys::f3;

                case VK_F4:
                    return keyboard::keys::f4;

                case VK_F5:
                    return keyboard::keys::f5;

                case VK_F6:
                    return keyboard::keys::f6;

                case VK_F7:
                    return keyboard::keys::f7;

                case VK_F8:
                    return keyboard::keys::f8;

                case VK_F9:
                    return keyboard::keys::f9;

                case VK_F10:
                    return keyboard::keys::f10;

                case VK_F11:
                    return keyboard::keys::f11;

                case VK_F12:
                    return keyboard::keys::f12;
                }

                return keyboard::keys::any;
            }
        } // namespace win

    } // namespace impl
} // namespace rtl
