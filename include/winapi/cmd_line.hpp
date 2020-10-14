// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#pragma once

#include "error.hpp"

#include <winapi/utf8.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/config.hpp>

// clang-format off
#include <windows.h>
#include <shellapi.h>
// clang-format on

#include <cstddef>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace winapi {

class CommandLine {
public:
    static CommandLine query() { return build_from_string(::GetCommandLineW()); }

    static CommandLine build_from_main(int argc, wchar_t* argv[]) {
        if (argc < 1)
            throw std::range_error{"invalid argc value"};

        std::string argv0{narrow(argv[0])};
        --argc;
        ++argv;

        std::vector<std::string> args;
        args.reserve(argc);

        for (int i = 0; i < argc; ++i)
            args.emplace_back(narrow(argv[i]));

        return {std::move(argv0), std::move(args)};
    }

    CommandLine() = default;

    bool has_argv0() const { return !argv0.empty(); }

    std::string get_argv0() const { return argv0; }

    std::string escape_argv0() const { return escape(get_argv0()); }

    bool has_args() const { return !get_args().empty(); }

    const std::vector<std::string>& get_args() const { return args; }

    std::vector<std::string> escape_args() const {
        std::vector<std::string> safe;
        safe.reserve(args.size());
        for (const auto& arg : args)
            safe.emplace_back(escape(arg));
        return safe;
    }

    static BOOST_CONSTEXPR char sep() { return ' '; }

    std::string join_args() const {
        return boost::algorithm::join(escape_args(), std::string{sep()});
    }

    std::string join() const {
        if (!has_argv0())
            throw std::logic_error{"argv[0] isn't defined"};
        std::ostringstream oss;
        oss << escape_argv0();
        if (has_args())
            oss << sep() << join_args();
        return oss.str();
    }

private:
    static CommandLine build_from_string(const std::string& src) {
        return build_from_string(widen(src));
    }

    static CommandLine build_from_string(std::wstring src) {
        boost::trim(src);
        if (src.empty())
            return {};

        int argc = 0;
        std::unique_ptr<wchar_t*, LocalDelete> argv{::CommandLineToArgvW(src.c_str(), &argc)};

        if (argv.get() == NULL)
            throw error::windows(GetLastError(), "CommandLineToArgvW");

        if (argc == 0)
            return {};

        std::string argv0{narrow(argv.get()[0])};

        std::vector<std::string> args;
        args.reserve(argc - 1);

        for (int i = 1; i < argc; ++i)
            args.emplace_back(narrow(argv.get()[i]));

        return {std::move(argv0), std::move(args)};
    }

    struct LocalDelete {
        void operator()(wchar_t* argv[]) const { ::LocalFree(argv); }
    };

    static std::string escape_for_cmd(const std::string& arg) {
        BOOST_STATIC_CONSTEXPR auto escape_symbol = '^';
        static const std::string dangerous_symbols{"^!\"%&()<>|"};

        auto safe = escape(arg);
        for (const auto danger : dangerous_symbols) {
            std::ostringstream replacement;
            replacement << escape_symbol << danger;
            boost::replace_all(safe, std::string{danger}, replacement.str());
        }
        return safe;
    }

    static std::string escape(const std::string& arg) {
        std::string safe;
        // Including the quotes:
        safe.reserve(arg.length() + 2);

        safe.push_back('"');

        for (auto it = arg.cbegin(); it != arg.cend(); ++it) {
            std::size_t numof_backslashes = 0;

            for (; it != arg.cend() && *it == '\\'; ++it)
                ++numof_backslashes;

            if (it == arg.cend()) {
                safe.reserve(safe.capacity() + numof_backslashes);
                safe.append(2 * numof_backslashes, '\\');
                break;
            }

            switch (*it) {
                case L'"':
                    safe.reserve(safe.capacity() + numof_backslashes + 1);
                    safe.append(2 * numof_backslashes + 1, '\\');
                    break;

                default:
                    safe.append(numof_backslashes, '\\');
                    break;
            }

            safe.push_back(*it);
        }

        safe.push_back('"');
        return safe;
    }

    CommandLine(std::vector<std::string>&& args) : args{std::move(args)} {}

    CommandLine(std::string&& argv0, std::vector<std::string>&& args = {})
        : argv0{std::move(argv0)}, args{std::move(args)} {}

    const std::string argv0;
    const std::vector<std::string> args;
};

} // namespace winapi
