// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#pragma once

#include "command.hpp"

#include <winapi/process.hpp>

#include <windows.h>

#include <exception>
#include <string>
#include <utility>
#include <vector>

namespace worker {

class Worker {
public:
    Worker(winapi::Process&& process) : m_cmd{Command::create()}, m_process{std::move(process)} {}

    ~Worker() {
        try {
            if (m_process.is_running()) {
                exit();
            }
        } catch (const std::exception&) {
        }
    }

    HWND get_console_window() {
        HWND ret = NULL;
        m_cmd->get_result(Command::GET_CONSOLE_WINDOW,
                          [&ret](const Command::Result& result) { ret = result.console_window; });
        return ret;
    }

    bool is_window_visible() {
        bool ret = false;
        m_cmd->get_result(Command::IS_WINDOW_VISIBLE, [&ret](const Command::Result& result) {
            ret = result.is_window_visible;
        });
        return ret;
    }

    StdHandles get_std_handles() {
        StdHandles ret;
        m_cmd->get_result(Command::GET_STD_HANDLES,
                          [&ret](const Command::Result& result) { ret = result.std_handles; });
        return ret;
    }

    StdHandles test_write() {
        StdHandles ret;
        m_cmd->get_result(Command::TEST_WRITE,
                          [&ret](const Command::Result& result) { ret = result.std_handles; });
        return ret;
    }

    std::vector<std::string> read_last_lines(std::size_t numof_lines) {
        std::vector<std::string> ret;
        const auto set_args = [numof_lines](Command::Args& args) {
            args.numof_lines = numof_lines;
        };
        const auto read_result = [&ret](const Command::Result& result) {
            ret = result.console_buffer.extract();
        };
        m_cmd->get_result(Command::GET_CONSOLE_BUFFER, set_args, read_result);
        return ret;
    }

    int exit() {
        m_cmd->get_result(Command::EXIT);
        m_process.wait();
        return m_process.get_exit_code();
    }

private:
    Command::Shared m_cmd;
    winapi::Process m_process;
};

} // namespace worker
