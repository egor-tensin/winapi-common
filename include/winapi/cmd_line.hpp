// Copyright (c) 2016 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "Privilege check" project.
// For details, see https://github.com/egor-tensin/privilege-check.
// Distributed under the MIT License.

#pragma once

#include "error.hpp"
#include "string.hpp"

#include <Windows.h>
#include <shellapi.h>

#include <cstddef>

#include <memory>
#include <stdexcept>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

class CommandLine
{
public:
    static CommandLine query()
    {
        return build_from_string(GetCommandLine());
    }

    static CommandLine build_from_main(int argc, wchar_t* argv[])
    {
        if (argc < 1)
            throw std::range_error{"invalid argc value"};

        std::wstring argv0{argv[0]};
        --argc;
        ++argv;

        std::vector<std::wstring> args;
        args.reserve(argc);

        for (int i = 0; i < argc; ++i)
            args.emplace_back(argv[i]);

        return {std::move(argv0), std::move(args)};
    }

    CommandLine() = default;

    bool has_argv0() const
    {
        return !argv0.empty();
    }

    std::wstring get_argv0() const
    {
        return argv0;
    }

    std::wstring escape_argv0() const
    {
        return escape(get_argv0());
    }

    bool has_args() const
    {
        return !get_args().empty();
    }

    const std::vector<std::wstring>& get_args() const
    {
        return args;
    }

    std::vector<std::wstring> escape_args() const
    {
        std::vector<std::wstring> safe;
        safe.reserve(args.size());
        for (const auto& arg : args)
            safe.emplace_back(escape(arg));
        return safe;
    }

    static constexpr wchar_t sep() { return L' '; }

    std::wstring join_args() const
    {
        return string::join(sep(), escape_args());
    }

    std::wstring join() const
    {
        if (!has_argv0())
            throw std::logic_error{"argv[0] isn't defined"};
        std::wostringstream oss;
        oss << escape_argv0();
        if (has_args())
            oss << sep() << string::join(sep(), escape_args());
        return oss.str();
    }

private:
    static CommandLine build_from_string(std::wstring src)
    {
        string::trim(src);
        if (src.empty())
            return {};

        int argc = 0;
        std::unique_ptr<wchar_t*, LocalDelete> argv{CommandLineToArgvW(src.c_str(), &argc)};

        if (argv.get() == NULL)
            error::raise("CommandLineToArgvW");

        if (argc == 0)
            return {};

        std::wstring argv0{argv.get()[0]};

        std::vector<std::wstring> args;
        args.reserve(argc - 1);

        for (int i = 1; i < argc; ++i)
            args.emplace_back(argv.get()[i]);

        return {std::move(argv0), std::move(args)};
    }

    inline std::wstring escape_for_cmd(const std::wstring& arg)
    {
        static constexpr auto escape_symbol = L'^';
        static constexpr auto dangerous_symbols = L"!\"%&()<>^|";

        auto safe = escape(arg);
        string::prefix_with(safe, dangerous_symbols, escape_symbol);
        return safe;
    }

    static std::wstring escape(const std::wstring& arg)
    {
        std::wstring safe;
        safe.reserve(arg.length() + 2);

        safe.push_back(L'"');

        for (auto it = arg.cbegin(); it != arg.cend(); ++it)
        {
            std::size_t numof_backslashes = 0;

            for (; it != arg.cend() && *it == L'\\'; ++it)
                ++numof_backslashes;

            if (it == arg.cend())
            {
                safe.reserve(safe.capacity() + numof_backslashes);
                safe.append(2 * numof_backslashes, L'\\');
                break;
            }

            switch (*it)
            {
                case L'"':
                    safe.reserve(safe.capacity() + numof_backslashes + 1);
                    safe.append(2 * numof_backslashes + 1, L'\\');
                    break;

                default:
                    safe.append(numof_backslashes, L'\\');
                    break;
            }

            safe.push_back(*it);
        }

        safe.push_back(L'"');
        return safe;
    }

    struct LocalDelete
    {
        void operator()(wchar_t* argv[]) const
        {
            LocalFree(argv);
        }
    };

    CommandLine(std::vector<std::wstring>&& args)
        : args{std::move(args)}
    { }

    CommandLine(std::wstring&& argv0, std::vector<std::wstring>&& args = {})
        : argv0{std::move(argv0)}
        , args{std::move(args)}
    { }

    const std::wstring argv0;
    const std::vector<std::wstring> args;
};
