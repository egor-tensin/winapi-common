// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#include <winapi/buffer.hpp>
#include <winapi/error.hpp>
#include <winapi/handle.hpp>
#include <winapi/utils.hpp>

#include <boost/config.hpp>

#include <windows.h>

#include <cassert>
#include <cstddef>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace winapi {
namespace {

std::runtime_error write_file_incomplete(std::size_t expected, std::size_t actual) {
    std::ostringstream oss;
    oss << "WriteFile could only write " << actual << " bytes instead of " << expected;
    return std::runtime_error{oss.str()};
}

bool is_std_handle(HANDLE handle) {
    return handle == ::GetStdHandle(STD_INPUT_HANDLE) ||
           handle == ::GetStdHandle(STD_OUTPUT_HANDLE) ||
           handle == ::GetStdHandle(STD_ERROR_HANDLE);
}

} // namespace

Handle::Handle(HANDLE impl) : m_impl(impl) {}

Handle::Handle(Handle&& other) BOOST_NOEXCEPT_OR_NOTHROW {
    swap(other);
}

Handle& Handle::operator=(Handle other) BOOST_NOEXCEPT_OR_NOTHROW {
    swap(other);
    return *this;
}

void Handle::swap(Handle& other) BOOST_NOEXCEPT_OR_NOTHROW {
    using std::swap;
    swap(m_impl, other.m_impl);
}

bool Handle::is_valid() const {
    return m_impl && is_valid(m_impl.get());
}

bool Handle::is_valid(HANDLE handle) {
    return handle != NULL && handle != INVALID_HANDLE_VALUE;
}

void Handle::close() {
    m_impl.reset();
}

bool Handle::is_std() const {
    return is_std_handle(m_impl.get());
}

Handle Handle::std_in() {
    return Handle{::GetStdHandle(STD_INPUT_HANDLE)};
}

Handle Handle::std_out() {
    return Handle{::GetStdHandle(STD_OUTPUT_HANDLE)};
}

Handle Handle::std_err() {
    return Handle{::GetStdHandle(STD_ERROR_HANDLE)};
}

bool Handle::read_chunk(Buffer& buffer) const {
    buffer.resize(max_chunk_size);
    DWORD nb_read = 0;

    if (buffer.size() > std::numeric_limits<DWORD>::max())
        throw std::range_error{"Read buffer is too large"};
    const auto ret =
        ::ReadFile(m_impl.get(), buffer.data(), static_cast<DWORD>(buffer.size()), &nb_read, NULL);

    buffer.resize(nb_read);

    if (ret) {
        return nb_read != 0;
    }

    const auto ec = GetLastError();

    switch (ec) {
        case ERROR_BROKEN_PIPE:
            // We've been reading from an anonymous pipe, and it's been closed.
            return false;
        default:
            throw error::windows(ec, "ReadFile");
    }
}

Buffer Handle::read() const {
    Buffer buffer;

    while (true) {
        Buffer chunk;
        const auto next = read_chunk(chunk);
        buffer.add(chunk);

        if (!next) {
            break;
        }
    }

    return buffer;
}

void Handle::write(const void* data, std::size_t nb) const {
    DWORD nb_written = 0;

    if (nb > std::numeric_limits<DWORD>::max())
        throw std::range_error{"Write buffer is too large"};
    const auto ret = ::WriteFile(m_impl.get(), data, static_cast<DWORD>(nb), &nb_written, NULL);

    if (!ret) {
        throw error::windows(GetLastError(), "WriteFile");
    }

    if (nb != nb_written) {
        throw write_file_incomplete(nb, nb_written);
    }
}

void Handle::write(const Buffer& buffer) const {
    write(buffer.data(), buffer.size());
}

void Handle::inherit(bool yes) const {
    if (!::SetHandleInformation(m_impl.get(), HANDLE_FLAG_INHERIT, yes ? 1 : 0)) {
        throw error::windows(GetLastError(), "SetHandleInformation");
    }
}

void Handle::Close::operator()(HANDLE impl) const {
    if (!is_valid(impl) || is_std_handle(impl))
        return;
    const auto ret = ::CloseHandle(impl);
    assert(ret);
    WINAPI_UNUSED_PARAMETER(ret);
}

} // namespace winapi
