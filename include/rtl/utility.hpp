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

#include <rtl/type_traits.hpp>

namespace rtl
{
    template<typename T>
    [[nodiscard]] constexpr typename remove_reference<T>::type&& move( T&& r )
    {
        return static_cast<typename remove_reference<T>::type&&>( r );
    }

    template<typename T>
    [[nodiscard]] constexpr T&& forward( typename remove_reference<T>::type& r )
    {
        return static_cast<T&&>( r );
    }

    template<typename T>
    [[nodiscard]] constexpr T&& forward( typename remove_reference<T>::type&& r )
    {
        static_assert( !is_lvalue_reference<T>::value, "attempt to forward rvalue as lvalue" );
        return static_cast<T&&>( r );
    }
} // namespace rtl
