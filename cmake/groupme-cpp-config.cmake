include(FindPkgConfig)

pkg_check_modules(SSL openssl REQUIRED)

pkg_check_modules(NLOHMANN-JSON nlohmann_json REQUIRED)

pkg_check_modules(LIBAVCODEC libavcodec REQUIRED)

pkg_check_modules(LIBAVFORMAT libavformat REQUIRED)

find_package(cpprestsdk REQUIRED CONFIG)

include(${CMAKE_CURRENT_LIST_DIR}/groupmeTargets.cmake)

install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/groupme-cpp-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/groupme-cpp-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_DATADIR}/groupme-cpp
)
