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
    template<typename T1, typename T2>
    struct pair
    {
        T1 first;
        T2 second;
    };

    template<typename T1, typename T2>
    inline pair<T1, T2> make_pair( const T1& t1, const T2& t2 )
    {
        pair<T1, T2> p;
        p.first = t1;
        p.second = t2;
        return p;
    }

} // namespace rtl
