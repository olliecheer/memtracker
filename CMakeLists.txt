cmake_minimum_required(VERSION 3.22)
project(untitled)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

include_directories(${CMAKE_SOURCE_DIR})

add_library(memtracker memtracker.cpp)
add_library(ringbuf ringbuf.cpp)
add_library(shmbuf shmbuf.cpp shmbuf.h)
add_library(printer printer.cpp)

link_libraries(printer)

add_subdirectory(test)
add_executable(data_process data_process.cpp data_process.h)
