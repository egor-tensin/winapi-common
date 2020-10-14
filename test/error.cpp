// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#include <winapi/error.hpp>

#include <boost/test/unit_test.hpp>

#include <windows.h>

#include <string>

BOOST_AUTO_TEST_SUITE(error_tests)

BOOST_AUTO_TEST_CASE(file_not_found) {
    const std::string actual{winapi::error::windows(ERROR_FILE_NOT_FOUND, "CreateFileW").what()};
    BOOST_TEST(actual ==
               "Function CreateFileW failed with error code 2: The system cannot find the file "
               "specified.");
}

BOOST_AUTO_TEST_SUITE_END()
