// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#pragma once

#include <string>
#include <utility>
#include <vector>

namespace winapi {

/**
 * @brief Command line for the current process or for launching new processes.
 *
 * This class takes care of proper parsing and stringifying command line
 * arguments so that they are safe to use with CreateProcess, ShellExecute,
 * etc.
 */
class CommandLine {
public:
    /** Get the command line used to launch this process. */
    static CommandLine query();

    /**
     * Parse a command line from a string.
     * @param src UTF-8 encoded string.
     */
    static CommandLine parse(const std::string& src);

    /**
     * Build a command line from main() arguments.
     * @param argc Length of the argv array.
     * @param argv UTF-16 encoded strings.
     */
    static CommandLine from_main(int argc, wchar_t* argv[]);

    /**
     * Build an empty command line.
     * It won't have neither argv[0], nor any other args.
     */
    CommandLine() = default;

    /**
     * Build a command line.
     * @param argv0 UTF-8 string, argv[0].
     * @param args  List of UTF-8 strings, other arguments.
     */
    explicit CommandLine(const std::string& argv0, const std::vector<std::string>& args = {})
        : m_argv0{argv0}, m_args{args} {}

    /**
     * Build a command line.
     * @param argv0 UTF-8 string, argv[0].
     * @param args  List of UTF-8 strings, other arguments.
     */
    explicit CommandLine(std::string&& argv0, std::vector<std::string>&& args = {})
        : m_argv0{std::move(argv0)}, m_args{std::move(args)} {}

    /**
     * Build a command line.
     * @param argv List of UTF-8 strings, including argv[0].
     */
    explicit CommandLine(std::vector<std::string> argv);

    static std::string escape(const std::string&);

    static std::string escape_cmd(const std::string&);

    /**
     * Build a string that represents this command line.
     * @return UTF-8 string.
     */
    std::string to_string() const;

    /**
     * Build a string that represents this command line, but omit argv[0].
     * @return UTF-8 string.
     */
    std::string args_to_string() const;

    /**
     * Get argv[0] for this command line.
     * @return UTF-8 string.
     */
    std::string get_argv0() const { return m_argv0; }

    /** Test if this command line has any additional arguments besides argv[0]. */
    bool has_args() const { return !get_args().empty(); }

    /**
     * Get list of arguments for this command line beyond argv[0].
     * @return List of UTF-8 strings.
     */
    const std::vector<std::string>& get_args() const { return m_args; }

    /**
     * Get list of arguments for this command line.
     * @return List of UTF-8 strings.
     */
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
