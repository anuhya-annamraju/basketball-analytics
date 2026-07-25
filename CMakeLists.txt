cmake_minimum_required(VERSION 3.10)
project(basketball-cpp-challenge VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    src/reader/playersnapshotreader.cpp
)
set(HEADERS
    src/reader/playersnapshotreader.hpp
)
include_directories(
    src/reader
)

add_library(basketball-lib ${SOURCES} ${HEADERS})
add_executable(basketball-app src/main.cpp)
target_link_libraries(basketball-app PRIVATE basketball-lib)

# Installation rules (optional)
#install(TARGETS basketball-app DESTINATION bin)