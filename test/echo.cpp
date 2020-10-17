// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

// Simple UTF-16 echo.

// clang-format off
#include <io.h>
#include <fcntl.h>
// clang-format on

#include <iostream>
#include <string>

int wmain(int argc, wchar_t* argv[]) {
    _setmode(_fileno(stdin), _O_U16TEXT);
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stderr), _O_U16TEXT);

    --argc;
    ++argv;

    if (argc > 0) {
        for (int i = 0; i < argc; ++i) {
            std::wcout << argv[i] << L'\n';
        }
    } else {
        std::wstring line;
        while (std::getline(std::wcin, line)) {
            std::wcout << line << L'\n';
        }
    }
    return 0;
}
