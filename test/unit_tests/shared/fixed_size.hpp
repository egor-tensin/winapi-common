// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#pragma once

#include <array>
#include <cstddef>
#include <cstring>
#include <string>
#include <vector>

namespace fixed_size {

/*
 * These are fixed-size classes, to be used as part of an interprocess shared
 * memory region.
 */

// 120 characters is an arbitrary limit, strings are cut to this many
// characters for storage.
template <std::size_t Length = 120>
class String : public std::array<char, Length> {
public:
    static String convert(const std::string& src) {
        String dest;
        std::size_t nch = dest.size() - 1;
        if (src.size() < nch) {
            nch = src.size();
        }
        std::memcpy(dest.data(), src.c_str(), nch);
        dest[nch] = '\0';
        return dest;
    }

    std::string extract() const {
        // Lines are null-terminated, and don't store their lenghts, so...
        return this->data();
    }
};

// 5 lines to store is also arbitrary, set it higher if needed.
template <std::size_t Length = 5, std::size_t StringLength = 120>
struct StringList : public std::array<String<StringLength>, Length> {
    static StringList convert(const std::vector<std::string>& src) {
        // If src.size() > Length, only the last Length lines from the source
        // list are stored.

        StringList dest;
        std::memset(&dest, 0, sizeof(dest));

        std::size_t src_offset = 0;
        if (src.size() > dest.size()) {
            src_offset = src.size() - dest.size();
        }

        for (std::size_t i = 0, j = src_offset; i < dest.size() && j < src.size();
             ++i, ++j, ++dest.numof_lines) {
            dest[i] = dest[i].convert(src[j]);
        }

        return dest;
    }

    std::vector<std::string> extract() const {
        std::vector<std::string> dest;
        for (std::size_t i = 0; i < numof_lines; ++i) {
            dest.emplace_back(this->at(i).extract());
        }
        return dest;
    }

    std::size_t numof_lines;
};

} // namespace fixed_size
