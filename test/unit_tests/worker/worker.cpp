// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#include "../shared/command.hpp"
#include "../shared/console.hpp"
#include "../shared/test_data.hpp"

#include <winapi/error.hpp>
#include <winapi/handle.hpp>

#include <windows.h>

#include <chrono>
#include <cstddef>
#include <exception>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

using worker::Command;

namespace {

bool is_window_visible() {
    HWND window = ::GetConsoleWindow();

    if (window == NULL) {
        return false;
    }

    const auto mask = ::GetWindowLongW(window, GWL_STYLE);

    if (!mask) {
        throw winapi::error::windows(GetLastError(), "GetWindowLongW");
    }

    return mask & WS_VISIBLE;
}

winapi::Handle write_to(winapi::Handle dest, const std::string& msg) {
    try {
        dest.write(msg + "\r\n");
    } catch (const std::exception&) {
        return winapi::Handle{};
    }
    return dest;
}

std::vector<std::string> get_console_buffer(std::size_t numof_lines) {
    return console::Buffer{}.read_last_lines(numof_lines);
}

void process_action(Command::Action action, const Command::Args& args, Command::Result& result) {
    switch (action) {
        case Command::EXIT:
            break;

        case Command::GET_CONSOLE_WINDOW:
            result.console_window = ::GetConsoleWindow();
            break;

        case Command::IS_WINDOW_VISIBLE:
            result.is_window_visible = is_window_visible();
            break;

        case Command::GET_STD_HANDLES:
            result.std_handles.in = ::GetStdHandle(STD_INPUT_HANDLE);
            result.std_handles.out = ::GetStdHandle(STD_OUTPUT_HANDLE);
            result.std_handles.err = ::GetStdHandle(STD_ERROR_HANDLE);
            break;

        case Command::TEST_WRITE:
            result.std_handles.in = winapi::Handle::std_in().get();
            result.std_handles.out =
                write_to(winapi::Handle::std_out(), worker::test_data::out()).get();
            result.std_handles.err =
                write_to(winapi::Handle::std_err(), worker::test_data::err()).get();
            break;

        case Command::GET_CONSOLE_BUFFER:
            result.console_buffer =
                result.console_buffer.convert(get_console_buffer(args.numof_lines));
            break;

        default:
            throw std::runtime_error{"invalid worker command"};
    }
}

int loop() {
    auto exit_loop = false;

    const auto cmd = Command::open();

    while (!exit_loop) {
        cmd->process_action([&exit_loop](Command::Action action,
                                         const Command::Args& args,
                                         Command::Result& result) {
            if (action == Command::EXIT) {
                exit_loop = true;
            } else {
                process_action(action, args, result);
            }
        });
    }
    return 0;
}

} // namespace

int main() {
    int ec = loop();
    std::this_thread::sleep_for(std::chrono::milliseconds{1000});
    return ec;
}
