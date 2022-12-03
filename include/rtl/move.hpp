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

namespace rtl
{
    namespace impl
    {
        template<typename T>
        struct remove_reference
        {
            typedef T type;
        };

        template<typename T>
        struct remove_reference<T&>
        {
            typedef T type;
        };

        template<typename T>
        struct remove_reference<T&&>
        {
            typedef T type;
        };
    } // namespace impl

    template<typename T>
    constexpr typename impl::remove_reference<T>::type&& move( T&& r )
    {
        return static_cast<typename impl::remove_reference<T>::type&&>( r );
    }
} // namespace rtl
