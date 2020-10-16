// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#include <winapi/cmd_line.hpp>
#include <winapi/pipe.hpp>
#include <winapi/process.hpp>
#include <winapi/utf8.hpp>

#include <boost/test/unit_test.hpp>

#include <stdexcept>
#include <string>
#include <utility>

using namespace winapi;

namespace {

class WithEchoExe {
public:
    WithEchoExe() : m_echo_exe(find_echo_exe()) {}

    const std::string& get_echo_exe() { return m_echo_exe; }

private:
    static std::string find_echo_exe() {
        static const std::string prefix{"--echo_exe="};
        return find_param_value(prefix);
    }

    static std::string find_param_value(const std::string& param_prefix) {
        const auto cmd_line = CommandLine::query();
        const auto& args = cmd_line.get_args();
        for (const auto& arg : args) {
            if (arg.rfind(param_prefix, 0) == 0) {
                return arg.substr(param_prefix.length());
            }
        }
        throw std::runtime_error{"couldn't find parameter " + param_prefix};
    }

    const std::string m_echo_exe;
};

} // namespace

BOOST_AUTO_TEST_SUITE(process_tests)

BOOST_FIXTURE_TEST_CASE(create_echo, WithEchoExe) {
    const CommandLine cmd_line{get_echo_exe()};
    auto process = Process::create(cmd_line);
    process.wait();
    BOOST_TEST(true, "Successfully created test process");
}

BOOST_FIXTURE_TEST_CASE(create_echo_with_args, WithEchoExe) {
    const CommandLine cmd_line{get_echo_exe(), {"1", "2", "3"}};
    auto process = Process::create(cmd_line);
    process.wait();
}

BOOST_FIXTURE_TEST_CASE(create_echo_pipe, WithEchoExe) {
    const CommandLine cmd_line{get_echo_exe(), {"1", "2", "3"}};
    Process::IO io;
    Pipe stdout_pipe;
    io.std_out = process::Stdout{stdout_pipe};
    auto process = Process::create(cmd_line, std::move(io));
    const auto output = stdout_pipe.read_end().read();
    process.wait();
    const auto utf8 = narrow(output);
    BOOST_TEST(utf8 == "1\r\n2\r\n3\r\n");
}

BOOST_AUTO_TEST_SUITE_END()
