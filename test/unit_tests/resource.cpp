// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#include "resource_ids.h"

#include <winapi/process.hpp>
#include <winapi/utf8.hpp>

#include <boost/format.hpp>
#include <boost/test/unit_test.hpp>

#include <cstdint>

using namespace winapi;

#include <ostream>
#include <vector>

namespace std {

ostream& operator<<(ostream& os, unsigned char c) {
    return os << boost::format("%|1$02x|") % static_cast<uint32_t>(c);
}

ostream& operator<<(ostream& os, const vector<unsigned char>& cs) {
    for (auto c : cs) {
        os << c;
    }
    return os;
}

} // namespace std

BOOST_TEST_SPECIALIZED_COLLECTION_COMPARE(Buffer);

BOOST_AUTO_TEST_SUITE(resource_tests)

BOOST_AUTO_TEST_CASE(get_string) {
    static constexpr auto expected = "This is a test resource string!";
    const auto actual = Process::get_resource_string(IDS_TEST_STRING);
    BOOST_TEST(actual == expected);
}

BOOST_AUTO_TEST_CASE(get_string_wide) {
    static constexpr auto expected = "This is another test string, wide this time.";
    const auto actual = Process::get_resource_string(IDS_TEST_STRING_WIDE);
    BOOST_TEST(actual == expected);
}

BOOST_AUTO_TEST_CASE(get_data) {
    static const Buffer expected{0xde, 0xad, 0xbe, 0xef};
    const auto actual = Process::get_resource(ID_TEST_DATA).copy();
    BOOST_TEST(actual == expected);
}

BOOST_AUTO_TEST_SUITE_END()
