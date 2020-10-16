// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#pragma once

#include "handle.hpp"

namespace winapi {

class Pipe {
public:
    Pipe();

    Handle& read_end() { return m_read_end; }
    const Handle& read_end() const { return m_read_end; }
    Handle& write_end() { return m_write_end; }
    const Handle& write_end() const { return m_write_end; }

private:
    Handle m_read_end;
    Handle m_write_end;
};

} // namespace winapi
