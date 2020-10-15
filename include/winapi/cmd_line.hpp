// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#pragma once

#include <boost/config.hpp>

#include <string>
#include <utility>
#include <vector>

namespace winapi {

class CommandLine {
public:
    static CommandLine query();

    static CommandLine build_from_main(int argc, wchar_t* argv[]);

    CommandLine() = default;

    CommandLine(const std::string& argv0, const std::vector<std::string>& args = {})
        : argv0(argv0), args(args) {}

    CommandLine(std::string&& argv0, std::vector<std::string>&& args = {})
        : argv0(std::move(argv0)), args(std::move(args)) {}

    bool has_argv0() const { return !argv0.empty(); }

    std::string get_argv0() const { return argv0; }

    std::string escape_argv0() const { return escape(get_argv0()); }

    bool has_args() const { return !get_args().empty(); }

    const std::vector<std::string>& get_args() const { return args; }

    std::vector<std::string> escape_args() const;

    static BOOST_CONSTEXPR char sep() { return ' '; }

    std::string join_args() const;

    std::string join() const;

private:
    static CommandLine build_from_string(const std::string&);

    static CommandLine build_from_string(std::wstring);

    static std::string escape_for_cmd(const std::string&);

    static std::string escape(const std::string&);

    CommandLine(std::vector<std::string>&& args) : args(std::move(args)) {}

    const std::string argv0;
    const std::vector<std::string> args;
};

} // namespace winapi
