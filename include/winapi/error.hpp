// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#pragma once

#include <windows.h>

#include <cstdint>
#include <sstream>
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

std::system_error windows(DWORD code, const char* function);

template <typename Ret>
std::runtime_error custom(Ret ret, const char* function) {
    std::ostringstream oss;
    oss << "Function " << function << " failed with error code " << ret;
    return std::runtime_error{oss.str()};
}

} // namespace error
} // namespace winapi
