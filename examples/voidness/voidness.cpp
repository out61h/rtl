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
#include <rtl/sys/application.hpp>
#include <rtl/sys/impl.hpp>

using rtl::Application;
using namespace rtl::keyboard;

void main()
{
    rtl::Application::instance().run(
        L"V O I D",
        nullptr,
        nullptr,
        []( [[maybe_unused]] const Application::Input& input,
            [[maybe_unused]] Application::Output&      output )
        {
            if ( input.keys.pressed[Keys::escape] )
                return Application::Action::close;

            return Application::Action::wait;
        },
        nullptr );
}
