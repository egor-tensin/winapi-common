// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#pragma once

#include "handle.hpp"

#include <cstddef>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>

namespace winapi {

/** @brief Named shared memory region. */
class SharedMemory {
public:
    /**
     * Creates a shared memory region.
     * @param name UTF-8 string.
     * @param nb   Number of bytes.
     */
    static SharedMemory create(const std::string& name, std::size_t nb);
    /**
     * Opens a shared memory region.
     * @param name UTF-8 string.
     */
    static SharedMemory open(const std::string& name);

    /** Get pointer to the data. */
    void* get() const { return m_addr.get(); }
    /** @overload */
    void* ptr() const { return get(); }

private:
    struct Unmap {
        void operator()(void*) const;
    };

    SharedMemory() = default;

    SharedMemory(Handle&& handle, void* addr) : m_handle{std::move(handle)}, m_addr{addr} {}

    Handle m_handle;
    std::unique_ptr<void, Unmap> m_addr;
};

/** @brief Easy way to represent a C++ object as a shared memory region. */
template <typename T>
class SharedObject {
public:
    typedef typename std::aligned_storage<sizeof(T), __alignof(T)>::type AlignedType;

    /**
     * Create the object & construct a shared memory region to store it.
     * @param name UTF-8 string, name of the shared memory region.
     * @param args Arguments to construct the object.
     */
    template <typename... Args>
    static SharedObject create(const std::string& name, Args&&... args) {
        SharedObject obj{SharedMemory::create(name, sizeof(AlignedType))};
        new (obj.ptr()) T(std::forward<Args>(args)...);
        obj.m_destruct = true;
        return obj;
    }

    /**
     * Open a shared memory region that stores the object.
     * @param name UTF-8 string, name of the shared memory region.
     */
    static SharedObject open(const std::string& name) {
        SharedObject obj{SharedMemory::open(name)};
        return obj;
    }

    SharedObject(SharedObject&& other) noexcept = default;
    SharedObject& operator=(const SharedObject& other) noexcept = default;
    SharedObject(const SharedObject&) = delete;

    ~SharedObject() {
        if (m_destruct && ptr()) {
            ptr()->~T();
        }
    }

    /** Get pointer to the object. */
    T* ptr() const { return reinterpret_cast<T*>(m_shmem.ptr()); }
    /** Get reference to the object. */
    T& get() const { return *ptr(); }

    T* operator->() const { return ptr(); }
    T& operator*() const { return get(); }

private:
    explicit SharedObject(SharedMemory&& shmem) : m_shmem(std::move(shmem)) {}

    SharedMemory m_shmem;
    // Destruct only once, no matter the number of mappings.
    bool m_destruct = false;
};

} // namespace winapi
