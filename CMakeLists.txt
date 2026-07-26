cmake_minimum_required(VERSION 3.10)
project(basketball-cpp-challenge VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(SOURCES
    src/reader/playersnapshotreader.cpp
    src/analytics/SnippetAnalyser.cpp
    src/streamprocessor/StreamProcessor.cpp
)
set(HEADERS
    src/reader/playersnapshotreader.hpp
    src/analytics/SnippetAnalyser.hpp
)

include_directories(
    src
    src/reader
    src/analytics
    src/streamprocessor
)
add_library(basketball-lib ${SOURCES})
add_executable(basketball-app src/main.cpp )
target_link_libraries(basketball-app PRIVATE basketball-lib)