// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#pragma once

#include <string>

namespace worker {
namespace test_data {

BOOST_STATIC_CONSTEXPR auto str = "Test output.";

inline std::string out() {
    return "stdout: " + std::string{str};
}

inline std::string err() {
    return "stderr: " + std::string{str};
}

} // namespace test_data
} // namespace worker
