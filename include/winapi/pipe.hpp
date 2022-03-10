// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#pragma once

#include "handle.hpp"

#include <utility>

namespace winapi {

/** @brief Anonymous pipe wrapper. */
class Pipe {
public:
    /** Create a new pipe. */
    Pipe();

    /** Get the read end of the pipe. */
    Handle& read_end() { return m_read_end; }
    /** @overload */
    const Handle& read_end() const { return m_read_end; }
    /** Get the write end of the pipe. */
    Handle& write_end() { return m_write_end; }
    /** @overload */
    const Handle& write_end() const { return m_write_end; }

private:
    Handle m_read_end;
    Handle m_write_end;
};

} // namespace winapi
