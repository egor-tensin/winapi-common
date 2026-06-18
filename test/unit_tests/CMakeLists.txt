find_package(Boost CONFIG REQUIRED COMPONENTS unit_test_framework)

file(GLOB shared_src CONFIGURE_DEPENDS "shared/*.hpp")

file(GLOB unit_tests_src CONFIGURE_DEPENDS "*.cpp" "*.h" "*.hpp" "*.rc")
add_executable(unit_tests ${unit_tests_src} ${shared_src})
set_target_properties(unit_tests PROPERTIES OUTPUT_NAME winapi-common-unit-tests)
target_link_libraries(unit_tests PRIVATE winapi_common winapi_utf8)
target_link_libraries(unit_tests PRIVATE
    Boost::disable_autolinking
    Boost::unit_test_framework
)
install(TARGETS unit_tests RUNTIME DESTINATION bin)

file(GLOB worker_src CONFIGURE_DEPENDS "worker/*.cpp")
add_executable(worker ${worker_src} ${shared_src})
set_target_properties(worker PROPERTIES OUTPUT_NAME winapi-common-test-worker)
target_link_libraries(worker PRIVATE winapi_common winapi_utf8)
target_link_libraries(worker PRIVATE
    Boost::disable_autolinking
    Boost::boost
)
install(TARGETS worker RUNTIME DESTINATION bin)

if(MSVC)
    install(FILES
        $<TARGET_PDB_FILE:unit_tests>
        $<TARGET_PDB_FILE:worker>
        DESTINATION bin OPTIONAL
    )
endif()

find_package(Python3 REQUIRED COMPONENTS Interpreter)

add_test(NAME unit_tests COMMAND unit_tests
    --log_level=all
    --no_color_output
    "--log_sink=$<TARGET_FILE_DIR:unit_tests>/unit_tests.log"
    "--report_sink=$<TARGET_FILE_DIR:unit_tests>/unit_tests_report.txt"
    --run_test=!console_tests,process_console_tests
    --
    "--echo_exe=$<TARGET_FILE:echo>"
    "--worker_exe=$<TARGET_FILE:worker>"
)

add_test(NAME unit_tests_report COMMAND Python3::Interpreter
    "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/tools/ctest-driver.py"
    grep
    --pass-regex [=[^\*\*\* No errors detected$]=]
    --
    "$<TARGET_FILE_DIR:unit_tests>/unit_tests_report.txt"
)

add_test(NAME console_tests COMMAND Python3::Interpreter
    "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/tools/ctest-driver.py"
    run
    --new-window
    --
    "$<TARGET_FILE:unit_tests>"
    --log_level=all
    --no_color_output
    "--log_sink=$<TARGET_FILE_DIR:unit_tests>/console_tests.log"
    "--report_sink=$<TARGET_FILE_DIR:unit_tests>/console_tests_report.txt"
    --run_test=console_tests,process_console_tests
    --
    "--echo_exe=$<TARGET_FILE:echo>"
    "--worker_exe=$<TARGET_FILE:worker>"
)

add_test(NAME console_tests_report COMMAND Python3::Interpreter
    "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/tools/ctest-driver.py"
    grep
    --pass-regex [=[^\*\*\* No errors detected$]=]
    --
    "$<TARGET_FILE_DIR:unit_tests>/console_tests_report.txt"
)
