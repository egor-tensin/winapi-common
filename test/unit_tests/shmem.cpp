// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-common" project.
// For details, see https://github.com/egor-tensin/winapi-common.
// Distributed under the MIT License.

#include <winapi/shmem.hpp>

#include <boost/test/unit_test.hpp>

#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <thread>

using namespace winapi;

namespace {

static constexpr auto shmem_name = "test-data-struct";

static constexpr int32_t main_data = -1;
static constexpr int32_t setter1_data = 69;
static constexpr int32_t setter2_data = 420;

struct DataStruct {
    std::mutex mtx;
    std::condition_variable cv;
    int32_t data;
};

void setter1_main() {
    const auto data_struct = SharedObject<DataStruct>::open(shmem_name);
    {
        std::unique_lock<std::mutex> lck{data_struct->mtx};
        data_struct->cv.wait(lck, [&]() { return data_struct->data == main_data; });
        data_struct->data = setter1_data;
    }
    data_struct->cv.notify_all();
}

void setter2_main() {
    auto data_struct = SharedObject<DataStruct>::open(shmem_name);
    {
        std::unique_lock<std::mutex> lck{data_struct->mtx};
        data_struct->cv.wait(lck, [&]() { return data_struct->data == setter1_data; });
        data_struct->data = setter2_data;
    }
    data_struct->cv.notify_all();
}

} // namespace

BOOST_AUTO_TEST_SUITE(shmem_tests)

BOOST_AUTO_TEST_CASE(basic) {
    auto data_struct = SharedObject<DataStruct>::create(shmem_name);

    std::thread setter1{&setter1_main};
    std::thread setter2{&setter2_main};
    {
        std::lock_guard<std::mutex> lck{data_struct->mtx};
        data_struct->data = main_data;
    }
    data_struct->cv.notify_all();

    setter1.join();
    setter2.join();

    BOOST_TEST(data_struct->data == setter2_data);
}

BOOST_AUTO_TEST_SUITE_END()
