// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#pragma once

#include <string>
#include <utility>
#include <vector>

namespace winapi {

class CommandLine {
public:
    static CommandLine query();

    static CommandLine parse(const std::string&);

    static CommandLine from_main(int argc, wchar_t* argv[]);

    CommandLine() = default;

    explicit CommandLine(const std::string& argv0, const std::vector<std::string>& args = {})
        : m_argv0{argv0}, m_args{args} {}

    explicit CommandLine(std::string&& argv0, std::vector<std::string>&& args = {})
        : m_argv0{std::move(argv0)}, m_args{std::move(args)} {}

    explicit CommandLine(std::vector<std::string> argv);

    static std::string escape(const std::string&);

    static std::string escape_cmd(const std::string&);

    std::string to_string() const;

    std::string args_to_string() const;

    std::string get_argv0() const { return m_argv0; }

    bool has_args() const { return !get_args().empty(); }

    const std::vector<std::string>& get_args() const { return m_args; }

    std::vector<std::string> get_argv() const;

private:
    static constexpr char token_sep() { return ' '; }

    std::string escape_argv0() const { return escape(get_argv0()); }

    std::vector<std::string> escape_args() const;

    std::vector<std::string> escape_argv() const;

    std::string m_argv0;
    std::vector<std::string> m_args;
};

} // namespace winapi
