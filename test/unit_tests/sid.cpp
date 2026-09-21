// Copyright (c) 2020 Egor Tensin <egor@tensin.name>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common
// Distributed under the MIT License.

#include <winapi/sid.hpp>

#include <boost/test/unit_test.hpp>

using namespace winapi;

BOOST_AUTO_TEST_SUITE(sid_tests)

BOOST_AUTO_TEST_CASE(builtin_administrators) {
    const auto sid = Sid::builtin_administrators();
    BOOST_TEST(sid.to_string() == "S-1-5-32-544");
}

BOOST_AUTO_TEST_SUITE_END()
