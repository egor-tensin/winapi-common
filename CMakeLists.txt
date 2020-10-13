cmake_minimum_required(VERSION 3.5) # for Boost::* imported targets

project(winapi_common CXX)

include(cmake/common.cmake)

find_package(Boost REQUIRED)

add_library(winapi_common INTERFACE)
target_include_directories(winapi_common INTERFACE include/)
target_link_libraries(winapi_common INTERFACE Boost::disable_autolinking Boost::boost)

install(DIRECTORY include/winapi DESTINATION include)

if(WINAPI_COMMON_TESTS)
    add_subdirectory(test)
endif()

install(FILES LICENSE.txt DESTINATION share)
