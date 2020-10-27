// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#include <winapi/file.hpp>
#include <winapi/handle.hpp>
#include <winapi/path.hpp>
#include <winapi/process_io.hpp>

#include <boost/config.hpp>

#include <string>
#include <utility>

namespace winapi {
namespace process {

Stdin::Stdin() : Stream(Handle::std_in()) {}

Stdout::Stdout() : Stream(Handle::std_out()) {}

Stderr::Stderr() : Stream(Handle::std_err()) {}

Stdin::Stdin(const std::string& path) : Stream(File::open_r(path)) {}

Stdin::Stdin(const CanonicalPath& path) : Stream(File::open_r(path)) {}

Stdout::Stdout(const std::string& path) : Stream(File::open_w(path)) {}

Stdout::Stdout(const CanonicalPath& path) : Stream(File::open_w(path)) {}

Stderr::Stderr(const std::string& path) : Stream(File::open_w(path)) {}

Stderr::Stderr(const CanonicalPath& path) : Stream(File::open_w(path)) {}

Stdin::Stdin(Pipe& pipe) : Stream(std::move(pipe.read_end())) {
    pipe.write_end().dont_inherit();
}

Stdout::Stdout(Pipe& pipe) : Stream(std::move(pipe.write_end())) {
    pipe.read_end().dont_inherit();
}

Stderr::Stderr(Pipe& pipe) : Stream(std::move(pipe.write_end())) {
    pipe.read_end().dont_inherit();
}

Stream::Stream(Stream&& other) BOOST_NOEXCEPT_OR_NOTHROW {
    swap(other);
}

Stream& Stream::operator=(Stream other) BOOST_NOEXCEPT_OR_NOTHROW {
    swap(other);
    return *this;
}

void Stream::swap(Stream& other) BOOST_NOEXCEPT_OR_NOTHROW {
    using std::swap;
    swap(handle, other.handle);
}

Stdin::Stdin(Stdin&& other) BOOST_NOEXCEPT_OR_NOTHROW : Stream(std::move(other)) {}

Stdin& Stdin::operator=(Stdin other) BOOST_NOEXCEPT_OR_NOTHROW {
    Stream::operator=(std::move(other));
    return *this;
}

Stdout::Stdout(Stdout&& other) BOOST_NOEXCEPT_OR_NOTHROW : Stream(std::move(other)) {}

Stdout& Stdout::operator=(Stdout other) BOOST_NOEXCEPT_OR_NOTHROW {
    Stream::operator=(std::move(other));
    return *this;
}

Stderr::Stderr(Stderr&& other) BOOST_NOEXCEPT_OR_NOTHROW : Stream(std::move(other)) {}

Stderr& Stderr::operator=(Stderr other) BOOST_NOEXCEPT_OR_NOTHROW {
    Stream::operator=(std::move(other));
    return *this;
}

IO::IO(IO&& other) BOOST_NOEXCEPT_OR_NOTHROW {
    swap(other);
}

IO& IO::operator=(IO other) BOOST_NOEXCEPT_OR_NOTHROW {
    swap(other);
    return *this;
}

void IO::swap(IO& other) BOOST_NOEXCEPT_OR_NOTHROW {
    using std::swap;
    swap(std_in, other.std_in);
    swap(std_out, other.std_out);
    swap(std_err, other.std_err);
}

void IO::close() {
    std_in.handle.close();
    std_out.handle.close();
    std_err.handle.close();
}

} // namespace process
} // namespace winapi
