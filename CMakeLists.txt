cmake_minimum_required(VERSION 3.13)

project(csvcalc)

set(CMAKE_BUILD_TYPE Debug Release)
set(CMAKE_CXX_STANDARD 20)

include_directories(./include)

set(SOURCES src/csvcalc.cpp src/csvparser.cpp)
set(TESTS src/csvparser.cpp src/testcsv.cpp)

add_executable(csvcalc ${SOURCES})
add_executable(testcsv ${TESTS})

target_link_libraries(testcsv PRIVATE gtest pthread)
