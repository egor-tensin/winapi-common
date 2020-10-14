// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#include <winapi/cmd_line.hpp>

#include <boost/test/unit_test.hpp>

#include <string>

BOOST_AUTO_TEST_SUITE(cmd_line_tests)

BOOST_AUTO_TEST_CASE(query) {
    const auto cmd_line = winapi::CommandLine::query();
    BOOST_TEST(cmd_line.has_argv0());
    BOOST_TEST_MESSAGE(cmd_line.get_argv0());
}

BOOST_AUTO_TEST_CASE(escape) {
    wchar_t* argv[] = {
        L"test.exe",
        L"arg1 arg2",
        LR"(path\to\file)",
        LR"(path\to\dir\)",
        LR"(weird\\argument)",
    };
    const auto cmd_line = winapi::CommandLine::build_from_main(5, argv);
    const auto expected =
        R"("test.exe" "arg1 arg2" "path\to\file" "path\to\dir\\" "weird\\argument")";
    BOOST_TEST(cmd_line.join() == expected);
}

BOOST_AUTO_TEST_SUITE_END()
