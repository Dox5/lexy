// Copyright (C) 2020-2021 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <lexy/input/file.hpp>
#include <nlohmann/json.hpp>

bool json_nlohmann(const lexy::buffer<lexy::utf8_encoding>& input)
{
    return nlohmann::json::accept(input.begin(), input.end());
}

