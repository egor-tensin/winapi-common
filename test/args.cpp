// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#include <iostream>

int wmain(int argc, wchar_t* argv[]) {
    for (int i = 0; i < argc; ++i) {
        std::wcout << argv[i] << L'\n';
    }
    return 0;
}
