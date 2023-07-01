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
    int wsprintf_s( wchar_t* buffer, size_t buffer_size, const wchar_t* fmt... );
    int wsprintf_s( wchar_t* buffer, size_t buffer_size, const char* fmt... );

    template<size_t Size, typename Char, typename... Args>
    int wsprintf_s( wchar_t ( &buffer )[Size], const Char* fmt, Args&&... args )
    {
        // TODO: rtl::forward<Args>(a)... ?
        return wsprintf_s( buffer, Size, fmt, args... );
    }
} // namespace rtl
