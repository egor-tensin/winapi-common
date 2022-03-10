// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#pragma once

#include "buffer.hpp"

#include <cstddef>

namespace winapi {

/** @brief Resources embedded in a PE (Portable Executable). */
struct Resource {
    // This is just a pointer to static data.

    Resource() = default;

    Resource(const void* data, std::size_t nb) : data{data}, nb{nb} {}

    /** Extract resource data into a Buffer instance. */
    Buffer copy() const { return {data, nb}; }

    const void* data = nullptr;
    std::size_t nb = 0;
};

} // namespace winapi
