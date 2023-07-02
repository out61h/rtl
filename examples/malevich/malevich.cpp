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

void main()
{
    rtl::Application::instance().run(
        L"Malevich",
        []( const rtl::Application::Environment&, rtl::Application::Params& params )
        {
            params.window = { 480, 480 };
            return true;
        },
        nullptr,
        nullptr,
        nullptr );
}
