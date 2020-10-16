// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#include "fixtures.hpp"

#include <winapi/cmd_line.hpp>
#include <winapi/file.hpp>
#include <winapi/path.hpp>
#include <winapi/pipe.hpp>
#include <winapi/process.hpp>
#include <winapi/utf8.hpp>

#include <boost/test/unit_test.hpp>

#include <utility>

using namespace winapi;
using namespace winapi::process;

BOOST_AUTO_TEST_SUITE(process_tests)

BOOST_FIXTURE_TEST_CASE(echo, WithEchoExe) {
    const CommandLine cmd_line{get_echo_exe()};
    const auto process = Process::create(cmd_line);
    process.wait();
    BOOST_TEST(process.get_exit_code() == 0);
}

BOOST_FIXTURE_TEST_CASE(echo_with_args, WithEchoExe) {
    const CommandLine cmd_line{get_echo_exe(), {"1", "2", "3"}};
    const auto process = Process::create(cmd_line);
    process.wait();
    BOOST_TEST(process.get_exit_code() == 0);
}

BOOST_FIXTURE_TEST_CASE(echo_stdout_to_pipe, WithEchoExe) {
    const CommandLine cmd_line{get_echo_exe(), {"aaa", "bbb", "ccc"}};
    Process::IO io;
    Pipe stdout_pipe;
    io.std_out = Stdout{stdout_pipe};
    const auto process = Process::create(cmd_line, std::move(io));
    const auto output = stdout_pipe.read_end().read();
    process.wait();
    BOOST_TEST(process.get_exit_code() == 0);
    const auto utf8 = narrow(output);
    BOOST_TEST(utf8 == "aaa\r\nbbb\r\nccc\r\n");
}

BOOST_FIXTURE_TEST_CASE(echo_stdout_to_file, WithEchoExe) {
    const CommandLine cmd_line{get_echo_exe(), {"XXX", "YYY", "ZZZ"}};
    Process::IO io;
    const CanonicalPath stdout_path{"test.txt"};
    const RemoveFileGuard remove_stdout_file{stdout_path};
    io.std_out = Stdout{stdout_path};
    const auto process = Process::create(cmd_line, std::move(io));
    process.wait();
    BOOST_TEST(process.get_exit_code() == 0);
    const auto output = File::open_r(stdout_path).read();
    const auto utf8 = narrow(output);
    BOOST_TEST(utf8 == "XXX\r\nYYY\r\nZZZ\r\n");
}

BOOST_AUTO_TEST_SUITE_END()
