// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#include <winapi/cmd_line.hpp>

// clang-format off
// The order matters for older Boost versions.
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
// clang-format on

#include <ostream>
#include <string>
#include <vector>

using namespace winapi;

namespace std {

ostream& operator<<(ostream& os, const vector<string>& xs) {
    os << "[\n";
    for (const auto& x : xs) {
        os << x << '\n';
    }
    os << "]";
    return os;
}

} // namespace std

BOOST_TEST_SPECIALIZED_COLLECTION_COMPARE(std::vector<std::string>);

namespace {

// MSDN examples
// https://docs.microsoft.com/en-us/cpp/c-language/parsing-c-command-line-arguments?view=vs-2019

const std::vector<std::string> msdn_string{
    R"(test.exe "abc" d e)",
    R"(test.exe a\\\b d"e f"g h)",
    R"(test.exe a\\\"b c d)",
    R"(test.exe a\\\\"b c" d e)",
};

const std::vector<std::vector<std::string>> msdn_argv{
    {"test.exe", "abc", "d", "e"},
    {"test.exe", R"(a\\\b)", "de fg", "h"},
    {"test.exe", R"(a\"b)", "c", "d"},
    {"test.exe", R"(a\\b c)", "d", "e"},
};

} // namespace

BOOST_AUTO_TEST_SUITE(cmd_line_tests)

BOOST_AUTO_TEST_CASE(query) {
    const auto cmd_line = CommandLine::query();
    BOOST_TEST(!cmd_line.get_argv0().empty(), "argv[0]: " << cmd_line.get_argv0());
}

BOOST_AUTO_TEST_CASE(to_string) {
    const std::vector<std::string> argv{
        "test.exe",
        "arg1 arg2",
        R"(path\to\file)",
        R"(path\to\dir\)",
        R"(weird\\argument)",
    };
    const CommandLine cmd_line{argv};
    const auto expected =
        R"("test.exe" "arg1 arg2" "path\to\file" "path\to\dir\\" "weird\\argument")";
    BOOST_TEST(cmd_line.to_string() == expected);
}

BOOST_DATA_TEST_CASE(msdn_parse,
                     boost::unit_test::data::make(msdn_string) ^ msdn_argv,
                     input,
                     expected) {
    const auto cmd_line = CommandLine::parse(input);
    const auto actual = cmd_line.get_argv();
    BOOST_TEST(actual == expected, "actual: " << actual);
}

BOOST_DATA_TEST_CASE(msdn_to_string, msdn_argv, argv) {
    const CommandLine cmd_line{argv};
    const auto actual = CommandLine::parse(cmd_line.to_string()).get_argv();
    BOOST_TEST(actual == argv, "actual: " << actual);
}

BOOST_AUTO_TEST_SUITE_END()
