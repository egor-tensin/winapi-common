// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#pragma once

#include <windows.h>

#include <cassert>

#define WINAPI_UNUSED_PARAMETER(...) (void)(__VA_ARGS__)

namespace winapi {

struct LocalDelete {
    void operator()(void* ptr) const {
        const auto ret = ::LocalFree(ptr);
        assert(ret == NULL);
        WINAPI_UNUSED_PARAMETER(ret);
    }
};

} // namespace winapi
