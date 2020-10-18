// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#pragma once

#include "buffer.hpp"

#include <boost/config.hpp>

#include <windows.h>

#include <cstddef>
#include <string>

namespace winapi {

class Sid {
public:
    BOOST_STATIC_CONSTEXPR std::size_t MAX_SID_SIZE = SECURITY_MAX_SID_SIZE;

    typedef SID Impl;

    static Sid well_known(WELL_KNOWN_SID_TYPE type);

    static Sid builtin_administrators();

    explicit Sid(const Buffer& buffer) : m_buffer(buffer) {}

    explicit operator SID&() { return get_impl(); }
    explicit operator const SID&() const { return get_impl(); }

    std::string to_string() const;

private:
    Impl& get_impl() { return *reinterpret_cast<SID*>(m_buffer.data()); }
    const Impl& get_impl() const { return *reinterpret_cast<const SID*>(m_buffer.data()); }

    Buffer m_buffer;
};

} // namespace winapi
