cmake_minimum_required(VERSION 3.5) # for Boost::* imported targets

project(winapi_common CXX)
enable_testing()

include(cmake/common.cmake)

find_package(Boost REQUIRED)
add_subdirectory(3rdparty/winapi/utf8 EXCLUDE_FROM_ALL)

file(GLOB_RECURSE winapi_common_include "include/*.hpp")
file(GLOB winapi_common_src "src/*.cpp")
add_library(winapi_common ${winapi_common_include} ${winapi_common_src})
target_include_directories(winapi_common PUBLIC include)
target_link_libraries(winapi_common PRIVATE winapi_utf8)
target_link_libraries(winapi_common PUBLIC Boost::disable_autolinking Boost::boost)
install(TARGETS winapi_common ARCHIVE DESTINATION lib)
install(DIRECTORY include/winapi DESTINATION include)

if(WINAPI_COMMON_TESTS)
    add_subdirectory(test)
endif()

install(FILES LICENSE.txt DESTINATION share)
