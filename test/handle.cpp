// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#include <winapi/handle.hpp>

#include <boost/test/unit_test.hpp>

#include <windows.h>

BOOST_AUTO_TEST_SUITE(handle_tests)

BOOST_AUTO_TEST_CASE(null) {
    { winapi::Handle h{NULL}; }
    BOOST_TEST(true, "NULL handle closed successfully");
    { winapi::Handle h{INVALID_HANDLE_VALUE}; }
    BOOST_TEST(true, "INVALID_HANDLE_VALUE handle closed successfully");
}

BOOST_AUTO_TEST_SUITE_END()
