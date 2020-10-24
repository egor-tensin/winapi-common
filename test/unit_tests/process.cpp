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
#include <winapi/process_io.hpp>
#include <winapi/utf8.hpp>

#include <boost/test/unit_test.hpp>

#include <chrono>
#include <thread>
#include <utility>

using namespace winapi;
using namespace winapi::process;

BOOST_AUTO_TEST_SUITE(process_tests)

BOOST_AUTO_TEST_CASE(get_exe_path) {
    const auto path = Process::get_exe_path();
    BOOST_TEST_MESSAGE("Executable path: " << path);
}

BOOST_FIXTURE_TEST_CASE(echo, WithEchoExe) {
    const CommandLine cmd_line{get_echo_exe(), {"1", "2", "3"}};
    const auto process = Process::create(cmd_line);
    process.wait();
    BOOST_TEST(process.get_exit_code() == 0);
}

BOOST_FIXTURE_TEST_CASE(echo_stdout_to_pipe, WithEchoExe) {
    const CommandLine cmd_line{get_echo_exe(), {"aaa", "bbb", "ccc"}};
    process::IO io;
    Pipe stdout_pipe;
    io.std_out = Stdout{stdout_pipe};
    const auto process = Process::create(cmd_line, std::move(io));
    const auto stdout16 = stdout_pipe.read_end().read();
    process.wait();
    BOOST_TEST(process.get_exit_code() == 0);
    const auto stdout8 = narrow(stdout16);
    BOOST_TEST(stdout8 == "aaa\r\nbbb\r\nccc\r\n");
}

BOOST_FIXTURE_TEST_CASE(echo_stdout_to_file, WithEchoExe) {
    static const CanonicalPath stdout_path{"test.txt"};
    const RemoveFileGuard remove_stdout_file{stdout_path};

    const CommandLine cmd_line{get_echo_exe(), {"XXX", "YYY", "ZZZ"}};
    process::IO io;
    io.std_out = Stdout{stdout_path};
    const auto process = Process::create(cmd_line, std::move(io));
    process.wait();
    BOOST_TEST(process.get_exit_code() == 0);
    const auto stdout16 = File::open_r(stdout_path).read();
    const auto stdout8 = narrow(stdout16);
    BOOST_TEST(stdout8 == "XXX\r\nYYY\r\nZZZ\r\n");
}

BOOST_FIXTURE_TEST_CASE(echo_stdin_from_file, WithEchoExe) {
    static const CanonicalPath stdin_path{"test.txt"};
    const RemoveFileGuard remove_stdin_file{stdin_path};
    static const std::string stdin8{"123\r\n456\r\n"};
    const auto stdin16 = widen(stdin8);
    File::open_w(stdin_path).write(stdin16);

    const CommandLine cmd_line{get_echo_exe()};
    process::IO io;
    Pipe stdout_pipe;
    io.std_in = Stdin{stdin_path};
    io.std_out = Stdout{stdout_pipe};
    const auto process = Process::create(cmd_line, std::move(io));
    const auto stdout16 = stdout_pipe.read_end().read();
    process.wait();
    BOOST_TEST(process.get_exit_code() == 0);
    const auto stdout8 = narrow(stdout16);
    BOOST_TEST(stdout8 == stdin8);
}

BOOST_FIXTURE_TEST_CASE(echo_runas, WithEchoExe) {
    const CommandLine cmd_line{get_echo_exe(), {"foo", "bar"}};
    const auto process = Process::runas(cmd_line);
    process.wait();
    BOOST_TEST(process.get_exit_code() == 0);
}

BOOST_FIXTURE_TEST_CASE(echo_terminate, WithEchoExe) {
    const CommandLine cmd_line{get_echo_exe()};
    const auto process = Process::create(cmd_line);

    // echo.exe is stuck trying to read stdin.
    BOOST_TEST(process.is_running());
    std::this_thread::sleep_for(std::chrono::seconds{3});
    BOOST_TEST(process.is_running());

    process.shut_down(123);

    BOOST_TEST(!process.is_running());
    BOOST_TEST(process.get_exit_code() == 123);
}

BOOST_AUTO_TEST_SUITE_END()
