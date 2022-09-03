cmake_minimum_required(VERSION 3.2)
include(FindPkgConfig)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(EmbreadedWM VERSION 0.1
DESCRIPTION "This is a library that provides a simple and easy way to communicate with the GroupMe API"
HOMEPAGE_URL "https://github.com/proatgram/libGroupMe-API"
LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

file(GLOB_RECURSE libGroupMe-API_SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE libGroupMe-API_HEADERS "${CMAKE_SOURCE_DIR}/include/*.h}" "${CMAKE_SOURCE_DIR}/include/*.hpp")

add_library(GroupMe SHARED libGroupMe-API_SOURCES)

target_include_directories(GroupMe PUBLIC libGroupMe-API_HEADERS)

add_executable(test "${CMAKE_SOURCE_DIR}/tests/main/src/*.cpp")

target_include_directories(test "${CMAKE_SOURCE_DIR}/tests/main/include/*h")
