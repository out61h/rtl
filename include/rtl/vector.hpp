#pragma once

#include <rtl/algorithm.hpp>
#include <rtl/memory.hpp>

namespace rtl
{
    template<typename T>
    class vector final
    {
    public:
        typedef T element_type;

        constexpr vector()
            : m_size( 0 )
            , m_capacity( default_capacity )
            , m_data( new element_type[default_capacity] )
        {
        }

        constexpr vector( size_t size, element_type&& value )
            : m_size( size )
            , m_capacity( rtl::min( size, default_capacity ) ) // TODO: do align?
            , m_data( new element_type[m_capacity] )
        {
            rtl::fill_n( m_data, value, m_size );
        }

        constexpr explicit vector( size_t size )
            : m_size( size )
            , m_capacity( rtl::min( size, default_capacity ) ) // TODO: do align?
            , m_data( new element_type[m_capacity] )
        {
            rtl::fill_n( m_data, m_size, element_type() );
        }

        ~vector()
        {
            delete[] m_data;
        }

        constexpr vector( const vector& that )
        {
            *this = that;
        }

        constexpr vector& operator=( const vector& that )
        {
            if ( this != &that )
            {
                if ( m_size != that.m_size )
                {
                    m_size = that.m_size;
                    m_capacity = that.m_capacity;

                    element_type* new_data = new element_type[that.m_capacity];
                    delete[] m_data;
                    m_data = new_data;
                }

                rtl::copy_n( that.m_data, that.m_size, m_data );
            }

            return *this;
        }

        constexpr vector( vector&& that )
        {
            *this = rtl::move( that );
        }

        constexpr vector& operator=( vector&& that )
        {
            if ( this != &that )
            {
                delete[] m_data;

                m_size = that.m_size;
                m_capacity = that.m_capacity;
                m_data = that.m_data;

                that.m_capacity = 0;
                that.m_size = 0;
                that.m_data = nullptr;
            }

            return *this;
        }

        [[nodiscard]] constexpr const element_type* data() const
        {
            return m_data;
        }

        [[nodiscard]] constexpr element_type* data()
        {
            return m_data;
        }

        [[nodiscard]] constexpr element_type& back()
        {
            return m_data[size() - 1];
        }

        [[nodiscard]] constexpr const element_type& back() const
        {
            return m_data[size() - 1];
        }

        [[nodiscard]] constexpr element_type* begin()
        {
            return m_data;
        }

        [[nodiscard]] constexpr element_type* end()
        {
            return m_data + m_size;
        }

        [[nodiscard]] constexpr const element_type* begin() const
        {
            return m_data;
        }

        [[nodiscard]] constexpr const element_type* end() const
        {
            return m_data + m_size;
        }

        [[nodiscard]] constexpr element_type& operator[]( size_t index )
        {
            return m_data[index];
        }

        [[nodiscard]] constexpr const element_type& operator[]( size_t index ) const
        {
            return m_data[index];
        }

        [[nodiscard]] constexpr size_t size() const
        {
            return m_size;
        }

        [[nodiscard]] constexpr size_t capacity() const
        {
            return m_capacity;
        }

        [[nodiscard]] constexpr bool empty() const
        {
            return m_size == 0;
        }

        // TODO: add const reference overload
        constexpr void push_back( element_type&& element )
        {
            if ( m_size + 1 > m_capacity )
            {
                if ( rtl::numeric_limits<size_t>::max() - m_capacity < m_capacity )
                    m_capacity = rtl::numeric_limits<size_t>::max(); // TODO: do align?
                else
                    m_capacity = m_capacity * 2;

                element_type* new_data = new element_type[m_capacity];

                rtl::copy_n( m_data, m_size, new_data );
                delete[] m_data;

                m_data = new_data;
            }

            m_data[m_size++] = rtl::move( element );
        }

        constexpr void resize( size_t size )
        {
            if ( m_size != size )
            {
                if ( size > m_capacity )
                {
                    m_capacity = size; // TODO: do align?

                    element_type* extended = new element_type[size];

                    rtl::copy_n( m_data, m_size, extended );
                    delete[] m_data;

                    m_data = extended;
                }

                if ( size > m_size )
                    rtl::fill_n( m_data + m_size, element_type(), size - m_size );

                m_size = size;
            }
        }

        constexpr void reserve( size_t capacity )
        {
            if ( m_capacity < capacity )
            {
                m_capacity = capacity; // TODO: do align?

                element_type* extended = new element_type[capacity];

                rtl::copy_n( m_data, m_size, extended );
                delete[] m_data;

                m_data = extended;
            }
        }

    private:
        static constexpr size_t default_capacity = 8;

        size_t        m_size;
        size_t        m_capacity;
        element_type* m_data;
    };
} // namespace rtl
