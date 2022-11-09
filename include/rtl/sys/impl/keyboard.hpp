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
                }

                return keyboard::keys::any;
            }
        } // namespace win

    } // namespace impl
} // namespace rtl
