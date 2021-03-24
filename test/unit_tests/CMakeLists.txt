file(GLOB shared_src "shared/*.hpp")

file(GLOB unit_tests_src "*.cpp" "*.h" "*.hpp" "*.rc")
add_executable(unit_tests ${unit_tests_src} ${shared_src})
set_target_properties(unit_tests PROPERTIES OUTPUT_NAME winapi-common-unit-tests)

target_link_libraries(unit_tests PRIVATE winapi_common winapi_utf8)

find_package(Boost REQUIRED COMPONENTS unit_test_framework)
target_link_libraries(unit_tests PRIVATE Boost::disable_autolinking Boost::unit_test_framework)

install(TARGETS unit_tests RUNTIME DESTINATION bin)
install_pdbs(TARGETS unit_tests DESTINATION bin)

file(GLOB worker_src "worker/*.cpp")
add_executable(worker ${worker_src} ${shared_src})
set_target_properties(worker PROPERTIES OUTPUT_NAME winapi-common-test-worker)

target_link_libraries(worker PRIVATE winapi_common winapi_utf8)

find_package(Boost REQUIRED)
target_link_libraries(worker PRIVATE Boost::disable_autolinking Boost::boost)

install(TARGETS worker RUNTIME DESTINATION bin)
install_pdbs(TARGETS worker DESTINATION bin)
