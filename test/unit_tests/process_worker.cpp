// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#include "fixtures.hpp"
#include "shared/command.hpp"
#include "shared/console.hpp"
#include "shared/test_data.hpp"
#include "shared/worker.hpp"

#include <winapi/buffer.hpp>
#include <winapi/cmd_line.hpp>
#include <winapi/handle.hpp>
#include <winapi/pipe.hpp>
#include <winapi/process.hpp>
#include <winapi/process_io.hpp>

#include <boost/test/unit_test.hpp>

#include <windows.h>

#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using winapi::Buffer;
using winapi::CommandLine;
using winapi::Handle;
using winapi::Pipe;
using winapi::Process;
using winapi::ProcessParameters;
namespace process = winapi::process;

using worker::StdHandles;
using worker::Worker;

namespace {

void check_window_same(Worker& worker) {
    BOOST_TEST(::GetConsoleWindow());
    BOOST_TEST(worker.get_console_window() == ::GetConsoleWindow());
    BOOST_TEST(worker.is_window_visible());
}

void check_window_none(Worker& worker) {
    BOOST_TEST(::GetConsoleWindow());
    BOOST_TEST(!worker.get_console_window());
    BOOST_TEST(!worker.is_window_visible());
}

void check_window_new(Worker& worker) {
    BOOST_TEST(::GetConsoleWindow());
    BOOST_TEST(worker.get_console_window());
    BOOST_TEST(worker.get_console_window() != ::GetConsoleWindow());
    BOOST_TEST(worker.is_window_visible());
}

void check_std_handles(Worker& worker, const StdHandles& expected) {
    const auto actual = worker.get_std_handles();
    BOOST_TEST(actual.in == expected.in);
    BOOST_TEST(actual.out == expected.out);
    BOOST_TEST(actual.err == expected.err);
}

void check_std_handles_same(Worker& worker) {
    check_std_handles(worker,
                      {::GetStdHandle(STD_INPUT_HANDLE),
                       ::GetStdHandle(STD_OUTPUT_HANDLE),
                       ::GetStdHandle(STD_ERROR_HANDLE)});
}

void check_std_handles_different(Worker& worker) {
    const auto actual = worker.get_std_handles();
    BOOST_TEST(actual.in);
    BOOST_TEST(actual.out);
    BOOST_TEST(actual.err);
    BOOST_TEST(actual.in != ::GetStdHandle(STD_INPUT_HANDLE));
    BOOST_TEST(actual.out != ::GetStdHandle(STD_OUTPUT_HANDLE));
    BOOST_TEST(actual.err != ::GetStdHandle(STD_ERROR_HANDLE));
}

void check_write(Worker& worker) {
    const auto handles = worker.test_write();
    BOOST_TEST(Handle::is_valid(handles.in));
    BOOST_TEST(Handle::is_valid(handles.out));
    BOOST_TEST(Handle::is_valid(handles.err));
}

void check_redirected_output(const Buffer& buffer, const std::vector<std::string>& expected_lines) {
    const auto actual = buffer.as_utf8();
    std::ostringstream oss;
    for (const auto& line : expected_lines) {
        oss << line << "\r\n";
    }
    const std::string expected{oss.str()};
    BOOST_TEST(actual == expected);
}

const std::string PLACEHOLDER{"This is a placeholder line."};

void check_console_buffer_inherit(Worker& worker, const std::vector<std::string>& expected) {
    // Check that
    // 1) the worker process writes the expected lines to its console window,
    // 2) the lines appear in this process's window, since they're the same.

    const auto numof_lines = expected.size();
    // Write some lines in this process's window, they should be overwritten
    // by worker's writes.
    for (std::size_t i = 0; i < numof_lines; ++i) {
        std::cout << PLACEHOLDER << std::endl;
    }
    // Order worker to write the lines:
    const auto handles = worker.test_write();
    // Query worker process's window:
    const auto worker_actual = worker.read_last_lines(numof_lines);
    // Query this process's window:
    const auto this_actual = console::Buffer{}.read_last_lines(numof_lines);

    // They should look the same:
    const auto& worker_expected = expected;
    const auto& this_expected = worker_expected;

    BOOST_TEST(worker_actual == worker_expected);
    BOOST_TEST(this_actual == this_expected);
}

void check_console_buffer_new(Worker& worker, const std::vector<std::string>& expected) {
    // Check that
    // 1) the worker process writes the expected lines in its console window,
    // 2) the lines _do not_ appear in this process's window, since they're
    // different windows.

    const auto numof_lines = expected.size();
    // Write some lines in this process's window, they _should not_ be
    // overwritten by worker's writes.
    for (std::size_t i = 0; i < numof_lines; ++i) {
        std::cout << PLACEHOLDER << std::endl;
    }
    // Order worker to write the lines:
    const auto handles = worker.test_write();
    // Query worker process's window:
    const auto worker_actual = worker.read_last_lines(numof_lines);
    // Query this process's window:
    const auto this_actual = console::Buffer{}.read_last_lines(numof_lines);

    // Placeholder lines are still last in this process's window:
    const auto& worker_expected = expected;
    const std::vector<std::string> this_expected(numof_lines, PLACEHOLDER);

    BOOST_TEST(worker_actual == worker_expected);
    BOOST_TEST(this_actual == this_expected);
}

// CREATE_NO_WINDOW actually does create a new buffer, it's just invisible.
void check_console_buffer_none(Worker& worker, const std::vector<std::string>& expected) {
    check_console_buffer_new(worker, expected);
}

} // namespace

BOOST_AUTO_TEST_SUITE(process_console_tests)

BOOST_FIXTURE_TEST_CASE(create_inherit, WithWorkerExe) {
    const CommandLine cmd_line{get_worker_exe()};
    ProcessParameters params{cmd_line};
    params.console_mode = ProcessParameters::ConsoleInherit;

    Worker worker{Process::create(std::move(params))};
    check_window_same(worker);
    check_std_handles_same(worker);
    check_write(worker);
    check_console_buffer_inherit(worker, {worker::test_data::out(), worker::test_data::err()});
    BOOST_TEST(worker.exit() == 0);
}

BOOST_FIXTURE_TEST_CASE(create_inherit_override, WithWorkerExe) {
    const CommandLine cmd_line{get_worker_exe()};
    ProcessParameters params{cmd_line};
    params.console_mode = ProcessParameters::ConsoleInherit;

    Pipe stdin_pipe, stderr_pipe;
    const StdHandles expected_handles{
        stdin_pipe.read_end().get(), Handle::std_out().get(), stderr_pipe.write_end().get()};

    process::IO io;
    io.std_in = process::Stdin{stdin_pipe};
    io.std_err = process::Stderr{stderr_pipe};
    params.io = std::move(io);

    Worker worker{Process::create(std::move(params))};
    check_window_same(worker);
    check_std_handles(worker, expected_handles);
    check_write(worker);
    check_console_buffer_inherit(worker, {worker::test_data::out()});
    BOOST_TEST(worker.exit() == 0);
    check_redirected_output(stderr_pipe.read_end().read(),
                            {worker::test_data::err(), worker::test_data::err()});
}

BOOST_FIXTURE_TEST_CASE(create_none, WithWorkerExe) {
    const CommandLine cmd_line{get_worker_exe()};
    ProcessParameters params{cmd_line};
    params.console_mode = ProcessParameters::ConsoleNone;

    Worker worker{Process::create(std::move(params))};
    check_window_none(worker);
    check_std_handles_different(worker);
    check_write(worker);
    check_console_buffer_none(worker, {worker::test_data::out(), worker::test_data::err()});
    BOOST_TEST(worker.exit() == 0);
}

BOOST_FIXTURE_TEST_CASE(create_none_override, WithWorkerExe) {
    const CommandLine cmd_line{get_worker_exe()};
    ProcessParameters params{cmd_line};
    params.console_mode = ProcessParameters::ConsoleNone;

    Pipe stdin_pipe, stderr_pipe;
    const StdHandles expected_handles{
        stdin_pipe.read_end().get(), Handle::std_out().get(), stderr_pipe.write_end().get()};

    process::IO io;
    io.std_in = process::Stdin{stdin_pipe};
    io.std_err = process::Stderr{stderr_pipe};
    params.io = std::move(io);

    Worker worker{Process::create(std::move(params))};
    check_window_none(worker);
    check_std_handles(worker, expected_handles);
    check_write(worker);
    check_console_buffer_none(worker, {worker::test_data::out()});
    BOOST_TEST(worker.exit() == 0);
    check_redirected_output(stderr_pipe.read_end().read(),
                            {worker::test_data::err(), worker::test_data::err()});
}

BOOST_FIXTURE_TEST_CASE(create_new, WithWorkerExe) {
    const CommandLine cmd_line{get_worker_exe()};
    ProcessParameters params{cmd_line};
    params.console_mode = ProcessParameters::ConsoleNew;

    Worker worker{Process::create(std::move(params))};
    check_window_new(worker);
    check_std_handles_different(worker);
    check_write(worker);
    check_console_buffer_new(worker, {worker::test_data::out(), worker::test_data::err()});
    BOOST_TEST(worker.exit() == 0);
}

BOOST_FIXTURE_TEST_CASE(create_new_override, WithWorkerExe) {
    const CommandLine cmd_line{get_worker_exe()};
    ProcessParameters params{cmd_line};
    params.console_mode = ProcessParameters::ConsoleNew;

    Pipe stdin_pipe, stderr_pipe;
    const StdHandles expected_handles{
        stdin_pipe.read_end().get(), Handle::std_out().get(), stderr_pipe.write_end().get()};

    process::IO io;
    io.std_in = process::Stdin{stdin_pipe};
    io.std_err = process::Stderr{stderr_pipe};
    params.io = std::move(io);

    Worker worker{Process::create(std::move(params))};
    check_window_new(worker);
    check_std_handles(worker, expected_handles);
    check_write(worker);
    check_console_buffer_new(worker, {worker::test_data::out()});
    BOOST_TEST(worker.exit() == 0);
    check_redirected_output(stderr_pipe.read_end().read(),
                            {worker::test_data::err(), worker::test_data::err()});
}

BOOST_AUTO_TEST_SUITE_END()
