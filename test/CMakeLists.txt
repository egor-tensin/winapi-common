add_executable(echo echo.cpp)
set_target_properties(echo PROPERTIES OUTPUT_NAME winapi-common-test-echo)

install(TARGETS echo RUNTIME DESTINATION bin)
install_pdbs(TARGETS echo DESTINATION bin)

add_subdirectory(unit_tests)
