file(GLOB COMMON_SOURCES "*.cpp")
add_library(common ${COMMON_SOURCES})
target_include_directories (common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
