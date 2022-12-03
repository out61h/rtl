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

#include <rtl/int.hpp>

namespace rtl
{
    template<typename T, T v>

    struct integral_constant
    {
        static constexpr T value = v;
        using value_type = T;
        using type = integral_constant;

        constexpr operator value_type() const noexcept
        {
            return value;
        }

        constexpr value_type operator()() const noexcept
        {
            return value;
        }
    };

    using true_type = integral_constant<bool, true>;
    using false_type = integral_constant<bool, false>;

    template<typename T, typename U>
    struct is_same : false_type
    {
    };

    template<typename T>
    struct is_same<T, T> : true_type
    {
    };

    template<typename T>
    struct is_array : false_type
    {
    };

    template<typename T>
    struct is_array<T[]> : true_type
    {
    };

    template<typename T, size_t N>
    struct is_array<T[N]> : true_type
    {
    };

    template<typename T>
    struct is_const : false_type
    {
    };

    template<typename T>
    struct is_const<const T> : true_type
    {
    };

    template<typename T>
    struct is_reference : false_type
    {
    };

    template<typename T>
    struct is_reference<T&> : true_type
    {
    };

    template<typename T>
    struct is_reference<T&&> : true_type
    {
    };

    template<typename T>
    struct is_function
        : integral_constant<bool, !is_const<const T>::value && !is_reference<T>::value>
    {
    };

    template<bool P, typename TrueType, typename FalseType>
    struct conditional
    {
        using type = TrueType;
    };

    template<typename TrueType, typename FalseType>
    struct conditional<false, TrueType, FalseType>
    {
        using type = FalseType;
    };

    template<typename T>
    struct type_identity
    {
        using type = T;
    };

    namespace impl
    {
        template<typename T>
        auto add_pointer( int ) -> type_identity<typename remove_reference<T>::type*>;

        template<typename T>
        auto add_pointer( ... ) -> type_identity<T>;

        template<typename T>
        auto add_lvalue_reference( int ) -> type_identity<T&>;

        template<typename T>
        auto add_lvalue_reference( ... ) -> type_identity<T>;

        template<class T>
        auto add_rvalue_reference( int ) -> type_identity<T&&>;

        template<class T>
        auto add_rvalue_reference( ... ) -> type_identity<T>;
    } // namespace impl

    template<typename T>
    struct add_pointer : decltype( impl::add_pointer<T>( 0 ) )
    {
    };

    template<class T>
    struct add_lvalue_reference : decltype( impl::add_lvalue_reference<T>( 0 ) )
    {
    };

    template<class T>
    struct add_rvalue_reference : decltype( impl::add_rvalue_reference<T>( 0 ) )
    {
    };

    template<typename T>
    struct remove_cv
    {
        using type = T;
    };

    template<typename T>
    struct remove_cv<const T>
    {
        using type = T;
    };

    template<typename T>
    struct remove_cv<volatile T>
    {
        using type = T;
    };

    template<typename T>
    struct remove_cv<const volatile T>
    {
        using type = T;
    };

    template<typename T>
    struct remove_reference
    {
        using type = T;
    };

    template<typename T>
    struct remove_reference<T&>
    {
        using type = T;
    };

    template<typename T>
    struct remove_reference<T&&>
    {
        using type = T;
    };

    template<typename T>
    struct remove_extent
    {
        using type = T;
    };

    template<typename T>
    struct remove_extent<T[]>
    {
        using type = T;
    };

    template<typename T, size_t N>
    struct remove_extent<T[N]>
    {
        using type = T;
    };

    template<typename T>
    struct is_floating_point
        : integral_constant<bool,
                            is_same<float, typename remove_cv<T>::type>::value
                                || is_same<double, typename remove_cv<T>::type>::value
                                || is_same<long double, typename remove_cv<T>::type>::value>
    {
    };

    // Applies conversions to the type T and removes cv-qualifiers:
    // - lvalue-to-rvalue;
    // - array-to-pointer;
    // - function-to-pointer.
    template<typename T>
    class decay
    {
    private:
        using U = typename remove_reference<T>::type;

    public:
        using type =
            typename conditional<is_array<U>::value,
                                 typename add_pointer<typename remove_extent<U>::type>::type,
                                 typename conditional<is_function<U>::value,
                                                      typename add_pointer<U>::type,
                                                      typename remove_cv<U>::type>::type>::type;
    };

    template<typename T>
    constexpr bool always_false = false;

    // Converts any type \T to a reference type, making it possible to use member functions in
    // \decltype expressions without the need to go through constructors.
    template<typename T>
    auto declval() -> typename add_rvalue_reference<T>::type;

    namespace impl
    {
        template<typename, typename, typename = void>
        struct decay_conditional_result
        {
        };

        // gives v͛͘̚ö́̈́̕i̔͛d̈́͊
        template<typename...>
        using void_type = void;

        // gives the common type of \T1 and \T2 using ternary operator's result type inferring
        template<typename T1, typename T2>
        using conditional_result_type = decltype( false ? declval<T1>() : declval<T2>() );

        // applies \decay conversion to the \conditional_result_type of \T1 and \T2
        template<typename T1, typename T2>
        struct decay_conditional_result<T1, T2, void_type<conditional_result_type<T1, T2>>>
            : decay<conditional_result_type<T1, T2>>
        {
        };

        template<typename T1, typename T2, typename = void>
        struct common_type : decay_conditional_result<const T1&, const T2&>
        {
        };
    } // namespace impl

    // TODO: Implement N-types generalization
    template<typename T1, typename T2>
    struct common_type
        : conditional<is_same<T1, typename decay<T1>::type>::value
                          && is_same<T2, typename decay<T2>::type>::value,
                      impl::common_type<T1, T2>,
                      common_type<typename decay<T1>::type, typename decay<T2>::type>>::type
    {
    };

    template<typename... T>
    using common_type_t = typename common_type<T...>::type;

} // namespace rtl
