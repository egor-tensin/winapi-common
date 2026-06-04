// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#pragma once

#include "handle.hpp"
#include "path.hpp"

#include <boost/functional/hash.hpp>

#include <windows.h>

#include <cstddef>
#include <functional>
#include <string_view>
#include <utility>

namespace winapi {

bool operator==(const FILE_ID_128& a, const FILE_ID_128& b);

/**
 * @brief File I/O.
 *
 * Easily open, read & write files.
 */
class File : public Handle {
public:
    struct ID {
        const FILE_ID_INFO impl;

        bool operator==(const ID& other) const {
            return impl.VolumeSerialNumber == other.impl.VolumeSerialNumber &&
                   impl.FileId == other.impl.FileId;
        }
    };

    /** Open file for reading. */
    static File open_r(std::string_view);
    /** @overload */
    static File open_r(const CanonicalPath&);
    /** Open file for reading (inc. ability to read its attributes). */
    static File open_read_attributes(std::string_view);
    /** @overload */
    static File open_read_attributes(const CanonicalPath&);
    /** Open file for writing. */
    static File open_w(std::string_view);
    /** @overload */
    static File open_w(const CanonicalPath&);

    /** Delete a file. */
    static void remove(std::string_view);
    /** @overload */
    static void remove(const CanonicalPath&);

    /** Make a File instance from an open handle. */
    explicit File(Handle&& handle) : Handle{std::move(handle)} {}

    /**
     * Get file size.
     * @return File size, bytes.
     */
    std::size_t get_size() const;

    /**
     * Get file ID.
     * File ID is a unique representation of a file, suitable for hashing.
     */
    ID query_id() const;
};

} // namespace winapi

namespace std {

template <>
struct hash<winapi::File::ID> {
    std::size_t operator()(const winapi::File::ID& id) const {
        std::size_t seed = 0;
        boost::hash_combine(seed, id.impl.VolumeSerialNumber);
        boost::hash_combine(seed, id.impl.FileId.Identifier);
        return seed;
    }
};

} // namespace std
