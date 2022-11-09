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

#include <rtl/algorithm.hpp>
#include <rtl/int.hpp>
#include <rtl/string.hpp>

namespace rtl
{
    namespace filesystem
    {
        // TODO: Implement a more complete analogue of std::filesystem
        class path final
        {
        public:
            path() = default;

            // cppcheck-suppress noExplicitConstructor
            path( const wstring_view& p )
                : m_path( p )
            {
            }

            const wchar_t* c_str() const
            {
                return m_path.c_str();
            }

            const wstring& wstring() const
            {
                return m_path;
            }

            path extension() const
            {
                return path( m_path.substr( m_path.rfind( L"." ) ) );
            }

            bool operator==( const path& rhs ) const
            {
                return m_path == rhs.m_path;
            }

        private:
            rtl::wstring m_path;
        };

        class directory_entry final
        {
        public:
            directory_entry() = default;

            explicit directory_entry( const path& p );

            void refresh();

            bool is_regular_file() const;
            bool is_directory() const;

            bool exists() const
            {
                return m_attributes != 0;
            }

            uintmax_t file_size() const
            {
                return m_file_size;
            }

            const path& path() const
            {
                return m_path;
            }

            bool operator==( const directory_entry& rhs ) const;

        private:
            friend class directory_iterator;

            filesystem::path m_path;
            uint32_t         m_pad1;
            uintmax_t        m_file_size = 0;
            uint32_t         m_attributes = 0;
            uint32_t         m_pad2;
        };

        class directory_iterator final
        {
        public:
            directory_iterator();

            explicit directory_iterator( const path& path );
            ~directory_iterator();

            directory_iterator& operator++();

            const directory_entry& operator*() const;

            bool operator==( const directory_iterator& rhs );
            bool operator!=( const directory_iterator& rhs );

            directory_iterator( const directory_iterator& ) = delete;
            directory_iterator& operator=( const directory_iterator& ) = delete;

            directory_iterator( directory_iterator&& other );
            directory_iterator& operator=( directory_iterator&& other );

        private:
            directory_entry m_entry;
            void*           m_handle;
            uint32_t        m_pad1;
        };

        size_t read_file_content( const wchar_t* name, void* p, size_t size );

    } // namespace filesystem
} // namespace rtl
