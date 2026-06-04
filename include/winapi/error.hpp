// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

/**
 * @file
 * @brief Make `std::system_error` work with `GetLastError()`.
 */

#pragma once

#include <windows.h>

#include <cstdint>
#include <format>
#include <stdexcept>
#include <string>
#include <system_error>

namespace winapi {
namespace error {

class CategoryWindows : public std::error_category {
public:
    CategoryWindows() = default;

    const char* name() const noexcept { return "Windows"; }

    std::string message(int32_t) const;
};

inline const CategoryWindows& category_windows() {
    static const CategoryWindows instance;
    return instance;
}

/**
 * Build a `std::system_error` from the value of `GetLastError()`.
 * @param code     Value of `GetLastError()`.
 * @param function Name of the function that failed, like "CreateFile".
 */
std::system_error windows(DWORD code, const char* function);

template <typename Ret>
std::runtime_error custom(Ret ret, const char* function) {
    return std::runtime_error{std::format("Function {} failed with error code {}", function, ret)};
}

} // namespace error
} // namespace winapi
