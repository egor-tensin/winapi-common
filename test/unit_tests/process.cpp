// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#include <winapi/cmd_line.hpp>
#include <winapi/process.hpp>

#include <boost/test/unit_test.hpp>

#include <stdexcept>
#include <string>

namespace {

class WhereTestExe {
public:
    WhereTestExe() : m_test_exe(find_test_exe()) {}

    const std::string& get_test_exe() { return m_test_exe; }

private:
    static std::string find_test_exe() {
        static const std::string param_prefix{"--test_exe="};
        const auto cmd_line = winapi::CommandLine::query();
        const auto& args = cmd_line.get_args();
        for (const auto& arg : args) {
            if (arg.rfind(param_prefix, 0) == 0) {
                return arg.substr(param_prefix.length());
            }
        }
        throw std::runtime_error{"couldn't find parameter --test_exe"};
    }

    const std::string m_test_exe;
};

} // namespace

BOOST_AUTO_TEST_SUITE(process_tests)

BOOST_AUTO_TEST_CASE(create_dir) {
    const winapi::CommandLine cmd_line{"cmd.exe", {"/c", "dir"}};
    auto process = winapi::Process::create(cmd_line);
    process.wait();
}

BOOST_FIXTURE_TEST_CASE(create_test_exe, WhereTestExe) {
    const winapi::CommandLine cmd_line{get_test_exe()};
    auto process = winapi::Process::create(cmd_line);
    process.wait();
    BOOST_TEST(true, "Successfully created test process");
}

BOOST_AUTO_TEST_SUITE_END()
