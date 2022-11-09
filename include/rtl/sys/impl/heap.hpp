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

#include "win.hpp"

namespace rtl
{
    namespace impl
    {
        class heap final
        {
        public:
            void init()
            {
                m_heap = ::GetProcessHeap();
                RTL_WINAPI_CHECK( m_heap != nullptr );
            }

            [[nodiscard]] void* calloc( size_t num, size_t size )
            {
                LPVOID result = ::HeapAlloc( m_heap, HEAP_ZERO_MEMORY, size * num );
                RTL_WINAPI_CHECK( result != nullptr );
                return result;
            }

            void free( void* ptr )
            {
                if ( !ptr )
                    return;

                [[maybe_unused]] const BOOL result = ::HeapFree( m_heap, 0, ptr );
                RTL_WINAPI_CHECK( result );
            }

        private:
            HANDLE m_heap;
        };
    } // namespace impl

} // namespace rtl
