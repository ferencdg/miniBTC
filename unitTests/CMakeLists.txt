file(GLOB UNITTESTS_SOURCES "*.cpp")
add_executable(unit_tests ${UNITTESTS_SOURCES})
target_link_libraries (unit_tests LINK_PUBLIC ${EXTERNAL_LIBS})
