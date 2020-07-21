cmake_minimum_required(VERSION 2.8)

project(simpleBlockchain)

find_package(Boost REQUIRED)
find_package(cpprestsdk REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST_LOG_DYN_LINK -fPIC")

set(EXTERNAL_LIBS cpprest boost_system boost_program_options sodium ssl crypto common boost_program_options gtest)

link_directories(/usr/local/lib/)
include_directories(${proxy_SOURCE_DIR} /usr/local/include)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -std=c++1y -pthread")

add_subdirectory(proto)
add_subdirectory(app)
add_subdirectory(common)
add_subdirectory(unitTests)
