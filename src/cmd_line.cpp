// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#include <winapi/cmd_line.hpp>
#include <winapi/error.hpp>
#include <winapi/utf8.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/config.hpp>

// clang-format off
#include <windows.h>
#include <shellapi.h>
// clang-format on

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include <sstream>
#include <utility>
#include <vector>

namespace winapi {
namespace {

struct LocalDelete {
    void operator()(wchar_t* argv[]) const { ::LocalFree(argv); }
};

CommandLine do_parse(std::wstring src) {
    boost::trim(src);
    if (src.empty()) {
        throw std::runtime_error{"Command line cannot be an empty string"};
    }

    int argc = 0;
    std::unique_ptr<wchar_t*, LocalDelete> argv{::CommandLineToArgvW(src.c_str(), &argc)};

    if (argv.get() == NULL)
        throw error::windows(GetLastError(), "CommandLineToArgvW");

    if (argc == 0) {
        throw std::runtime_error{"Command line must contain at least one token"};
    }

    std::string argv0{narrow(argv.get()[0])};

    std::vector<std::string> args;
    args.reserve(argc - 1);

    for (int i = 1; i < argc; ++i)
        args.emplace_back(narrow(argv.get()[i]));

    return {std::move(argv0), std::move(args)};
}

} // namespace

CommandLine CommandLine::query() {
    return do_parse(::GetCommandLineW());
}

CommandLine CommandLine::parse(const std::string& src) {
    return do_parse(widen(src));
}

CommandLine CommandLine::from_main(int argc, wchar_t* argv[]) {
    if (argc < 1)
        throw std::range_error{"argc must be a positive number"};

    std::string argv0{narrow(argv[0])};
    --argc;
    ++argv;

    std::vector<std::string> args;
    args.reserve(argc);

    for (int i = 0; i < argc; ++i)
        args.emplace_back(narrow(argv[i]));

    return {std::move(argv0), std::move(args)};
}

std::string CommandLine::escape(const std::string& arg) {
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

std::string CommandLine::escape_cmd(const std::string& arg) {
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

std::string CommandLine::to_string() const {
    std::ostringstream oss;
    oss << escape(get_argv0());
    if (has_args())
        oss << token_sep() << args_to_string();
    return oss.str();
}

std::string CommandLine::args_to_string() const {
    return boost::algorithm::join(escape_args(), std::string{token_sep()});
}

std::vector<std::string> CommandLine::escape_args() const {
    std::vector<std::string> safe;
    safe.reserve(args.size());
    for (const auto& arg : args)
        safe.emplace_back(escape(arg));
    return safe;
}

} // namespace winapi
