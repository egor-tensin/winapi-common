// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#pragma once

#include <boost/config.hpp>

#include <windows.h>

#include <string>
#include <system_error>

namespace winapi {
namespace error {

class CategoryWindows : public std::error_category {
public:
    CategoryWindows() = default;

    const char* name() const BOOST_NOEXCEPT_OR_NOTHROW { return "Windows"; }

    std::string message(int) const;
};

inline const CategoryWindows& category_windows() {
    static const CategoryWindows instance;
    return instance;
}

std::system_error windows(DWORD code, const char* function);

} // namespace error
} // namespace winapi
