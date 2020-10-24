// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#pragma once

#include "shared/command.hpp"

#include <winapi/cmd_line.hpp>
#include <winapi/file.hpp>
#include <winapi/path.hpp>

#include <boost/test/unit_test.hpp>

#include <exception>
#include <stdexcept>
#include <string>

class RemoveFileGuard {
public:
    explicit RemoveFileGuard(const winapi::CanonicalPath& path) : m_path(path) {}

    ~RemoveFileGuard() {
        try {
            winapi::File::remove(m_path);
        } catch (const std::exception& e) {
            BOOST_TEST_MESSAGE("Couldn't remove file " << m_path.get() << ": " << e.what());
        }
    }

private:
    winapi::CanonicalPath m_path;

    RemoveFileGuard(const RemoveFileGuard&) = delete;
    RemoveFileGuard& operator=(const RemoveFileGuard&) = delete;
};

class WithParam {
public:
    WithParam(const std::string& param_prefix) : m_value(find_param_value(param_prefix)) {}

    const std::string& get_value() { return m_value; }

private:
    static std::string find_param_value(const std::string& param_prefix) {
        const auto cmd_line = winapi::CommandLine::query();
        const auto& args = cmd_line.get_args();
        for (const auto& arg : args) {
            if (arg.rfind(param_prefix, 0) == 0) {
                return arg.substr(param_prefix.length());
            }
        }
        throw std::runtime_error{"couldn't find parameter " + param_prefix};
    }

    std::string m_value;
};

class WithEchoExe : public WithParam {
public:
    WithEchoExe() : WithParam{"--echo_exe="} {}

    const std::string& get_echo_exe() { return get_value(); }
};

class WithWorkerExe : public WithParam {
public:
    WithWorkerExe() : WithParam{"--worker_exe="}, m_cmd{worker::Command::create()} {}

    const std::string& get_worker_exe() { return get_value(); }

    worker::Command::Shared m_cmd;
};
