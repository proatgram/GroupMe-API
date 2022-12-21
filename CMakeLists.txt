cmake_minimum_required(VERSION 3.2)

include(FindPkgConfig)
include(CMakePackageConfigHelpers)

project(libGroupMe-cpp VERSION 0.1
DESCRIPTION "This is a library that provides a simple and easy way to communicate with the GroupMe API"
HOMEPAGE_URL "https://github.com/proatgram/libGroupMe-API"
LANGUAGES CXX)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_BUILD_TYPE Debug)

file(GLOB_RECURSE libGroupMe-API_SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE libGroupMe-API_HEADERS "${CMAKE_SOURCE_DIR}/include/*.h" "${CMAKE_SOURCE_DIR}/include/*.hpp")

pkg_check_modules(SSL openssl REQUIRED)

pkg_check_modules(NLOHMANN-JSON nlohmann_json REQUIRED)

pkg_check_modules(LIBAVCODEC libavcodec REQUIRED)

pkg_check_modules(LIBAVFORMAT libavformat REQUIRED)

find_package(cpprestsdk REQUIRED CONFIG)

add_library(GroupMe SHARED)

target_sources(GroupMe PRIVATE ${libGroupMe-API_SOURCES})

target_sources(GroupMe PUBLIC
    FILE_SET HEADERS
    BASE_DIRS include
    FILES ${libGroupMe-API_HEADERS}
)

target_link_libraries(GroupMe cpprestsdk::cpprest ${SSL_LINK_LIBRARIES} avformat)

add_executable(test "${CMAKE_SOURCE_DIR}/tests/main/src/main.cpp")

target_include_directories(test PRIVATE "${CMAKE_SOURCE_DIR}/tests/main/include/" "${CMAKE_SOURCE_DIR}/include")

target_link_libraries(test cpprestsdk::cpprest "${CMAKE_CURRENT_BINARY_DIR}/libGroupMe.so" ${SSL_LINK_LIBRARIES} avformat)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/groupme-cpp.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/groupme-cpp.pc
    @ONLY
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/groupme-cpp.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/groupme-cpp-config-version.cmake
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/groupme-cpp-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_DATADIR}/groupme-cpp
)

install(TARGETS GroupMe EXPORT groupmeTargets FILE_SET HEADERS)

install(EXPORT groupmeTargets NAMESPACE groupme-cpp:: DESTINATION ${CMAKE_INSTALL_DATADIR}/groupme-cpp)
