// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#pragma once

#include "fixed_size.hpp"

#include <winapi/shmem.hpp>

#include <boost/config.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

#include <windows.h>

#include <cstddef>
#include <exception>
#include <functional>
#include <stdexcept>

namespace worker {

struct StdHandles {
    HANDLE in;
    HANDLE out;
    HANDLE err;
};

class Command {
public:
    BOOST_STATIC_CONSTEXPR auto SHMEM_NAME = "shmem-test-cmd";

    typedef winapi::SharedObject<Command> Shared;

    static Shared create() { return Shared::create(SHMEM_NAME); }
    static Shared open() { return Shared::open(SHMEM_NAME); }

    typedef boost::interprocess::interprocess_mutex mutex;
    typedef boost::interprocess::interprocess_condition condition_variable;
    typedef boost::interprocess::scoped_lock<mutex> lock;

    enum Action {
        EXIT = 1,
        GET_CONSOLE_WINDOW,
        IS_WINDOW_VISIBLE,
        GET_STD_HANDLES,
        TEST_WRITE,
        GET_CONSOLE_BUFFER,
    };

    union Args {
        std::size_t numof_lines;
    };

    union Result {
        HWND console_window;
        bool is_window_visible;
        StdHandles std_handles;
        fixed_size::StringList<> console_buffer;
    };

    typedef std::function<void(Args&)> SetArgs;
    typedef std::function<void(const Result&)> ReadResult;

    void get_result(Action action, const SetArgs& set_args, const ReadResult& read_result) {
        {
            lock lck{m_mtx};
            m_action = action;
            set_args(m_args);

            m_action_requested = true;
            m_result_ready = false;
            m_error_occured = false;
        }
        m_cv.notify_all();

        lock lck{m_mtx};
        m_cv.wait(lck, [this]() { return m_error_occured || m_result_ready; });

        const auto error = m_error_occured;

        m_action_requested = false;
        m_result_ready = false;
        m_error_occured = false;

        if (error) {
            throw std::runtime_error{"Worker error: " + m_error.extract()};
        }

        read_result(m_result);
    }

    void get_result(Action action, const ReadResult& read_result) {
        return get_result(
            action, [](Args&) {}, read_result);
    }

    void get_result(Action action) {
        return get_result(action, [](const Result&) {});
    }

    typedef std::function<void(Action, const Args&, Result&)> ProcessAction;

    void process_action(const ProcessAction& callback) {
        {
            lock lck{m_mtx};
            m_cv.wait(lck, [this]() { return m_action_requested; });

            m_action_requested = false;
            m_result_ready = false;
            m_error_occured = false;

            try {
                callback(m_action, m_args, m_result);
                m_result_ready = true;
            } catch (const std::exception& e) {
                m_error_occured = true;
                m_error = m_error.convert(e.what());
            }
        }
        m_cv.notify_all();
    }

private:
    Action m_action;
    Args m_args;
    bool m_action_requested = false;

    Result m_result;
    bool m_result_ready = false;

    bool m_error_occured = false;
    fixed_size::String<> m_error;

    mutex m_mtx;
    condition_variable m_cv;
};

} // namespace worker
