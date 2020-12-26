// Copyright (C) 2020 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef TEST_TEST_ENCODING_HPP_INCLUDED
#define TEST_TEST_ENCODING_HPP_INCLUDED

#include <lexy/input/string_input.hpp>

struct test_encoding
{
    using char_type = char;
    struct int_type
    {
        int value;

        constexpr explicit operator int() const noexcept
        {
            return value;
        }
        constexpr explicit operator std::size_t() const noexcept
        {
            return std::size_t(value);
        }

        friend constexpr bool operator==(int_type a, int_type b) noexcept
        {
            return a.value == b.value;
        }
        friend constexpr bool operator!=(int_type a, int_type b) noexcept
        {
            return a.value != b.value;
        }

        friend constexpr bool operator<(int_type a, int_type b) noexcept
        {
            return a.value < b.value;
        }
        friend constexpr bool operator<=(int_type a, int_type b) noexcept
        {
            return a.value <= b.value;
        }
        friend constexpr bool operator>(int_type a, int_type b) noexcept
        {
            return a.value > b.value;
        }
        friend constexpr bool operator>=(int_type a, int_type b) noexcept
        {
            return a.value >= b.value;
        }
    };

    template <typename OtherCharType>
    static constexpr bool is_secondary_char_type = false;

    static LEXY_CONSTEVAL int_type eof()
    {
        return {lexy::default_encoding::eof()};
    }

    static LEXY_CONSTEVAL int_type to_int_type(char_type c) noexcept
    {
        return {lexy::default_encoding::to_int_type(c)};
    }
};

using test_input = lexy::string_input<test_encoding>;

#endif // TEST_TEST_ENCODING_HPP_INCLUDED

