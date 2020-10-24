// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#include "shared/console.hpp"

#include <boost/test/unit_test.hpp>

#include <iostream>
#include <string>
#include <vector>

BOOST_AUTO_TEST_SUITE(console_tests)

BOOST_AUTO_TEST_CASE(read_last_lines) {
    std::cout << "abc\ndef" << std::endl;
    console::Buffer buffer;
    const auto last = buffer.read_last_line();
    const auto last2 = buffer.read_last_lines(2);
    const std::string expected{"def"};
    const std::vector<std::string> expected2{"abc", "def"};
    BOOST_TEST(last == expected);
    BOOST_TEST(last2 == expected2);
}

BOOST_AUTO_TEST_CASE(read_last_lines_overflow) {
    console::Buffer buffer;
    const std::string output(buffer.get_columns() + 5, 'X');
    std::cout << output << std::endl;
    buffer.update();
    const auto last = buffer.read_last_lines(1);
    const auto last2 = buffer.read_last_lines(2);
    const std::vector<std::string> expected{"XXXXX"};
    const std::vector<std::string> expected2{std::string(buffer.get_columns(), 'X'), "XXXXX"};
    BOOST_TEST(last == expected);
    BOOST_TEST(last2 == expected2);
}

BOOST_AUTO_TEST_SUITE_END()
