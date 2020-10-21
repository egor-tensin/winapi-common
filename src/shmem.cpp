// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#include <winapi/error.hpp>
#include <winapi/handle.hpp>
#include <winapi/shmem.hpp>
#include <winapi/utf8.hpp>
#include <winapi/utils.hpp>

#include <boost/config.hpp>

#include <windows.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>

namespace winapi {
namespace {

void* do_map(const Handle& mapping, std::size_t nb = 0) {
    const auto addr = ::MapViewOfFile(static_cast<HANDLE>(mapping), FILE_MAP_ALL_ACCESS, 0, 0, nb);

    if (addr == NULL) {
        throw error::windows(GetLastError(), "MapViewOfFile");
    }

    return addr;
}

} // namespace

void SharedMemory::Unmap::operator()(void* ptr) const {
    const auto ret = ::UnmapViewOfFile(ptr);
    assert(ret);
    WINAPI_UNUSED_PARAMETER(ret);
};

SharedMemory SharedMemory::create(const std::string& name, std::size_t nb) {
    const auto nb64 = static_cast<std::uint64_t>(nb);
    static_assert(sizeof(nb64) == 2 * sizeof(DWORD), "sizeof(DWORD) != 32");

    const auto nb_low = static_cast<DWORD>(nb64);
    const auto nb_high = static_cast<DWORD>(nb64 >> 32);

    const auto mapping_impl = ::CreateFileMappingW(
        INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, nb_high, nb_low, widen(name).c_str());

    if (mapping_impl == NULL) {
        throw error::windows(GetLastError(), "CreateFileMappingW");
    }

    Handle mapping{mapping_impl};
    const auto addr = do_map(mapping);
    return {std::move(mapping), addr};
}

SharedMemory SharedMemory::open(const std::string& name) {
    const auto mapping_impl = ::OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, widen(name).c_str());

    if (mapping_impl == NULL) {
        throw error::windows(GetLastError(), "OpenFileMappingW");
    }

    Handle mapping{mapping_impl};
    const auto addr = do_map(mapping);
    return {std::move(mapping), addr};
}

SharedMemory::SharedMemory(SharedMemory&& other) BOOST_NOEXCEPT_OR_NOTHROW {
    swap(other);
}

SharedMemory& SharedMemory::operator=(SharedMemory other) BOOST_NOEXCEPT_OR_NOTHROW {
    swap(other);
    return *this;
}

void SharedMemory::swap(SharedMemory& other) BOOST_NOEXCEPT_OR_NOTHROW {
    using std::swap;
    swap(m_handle, other.m_handle);
    swap(m_addr, other.m_addr);
}

} // namespace winapi
