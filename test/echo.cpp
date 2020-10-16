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

int wmain(int argc, wchar_t* argv[]) {
    _setmode(_fileno(stdout), _O_U16TEXT);

    for (int i = 1; i < argc; ++i) {
        std::wcout << argv[i] << L'\n';
    }
    return 0;
}
