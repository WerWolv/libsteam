cmake_minimum_required(VERSION 3.21)
project(main)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_SHARED_LIBRARY_PREFIX "")

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBCURL REQUIRED IMPORTED_TARGET libcurl)

add_subdirectory(external/fmt)
add_subdirectory(external/json)
set_target_properties(fmt PROPERTIES POSITION_INDEPENDENT_CODE ON)
set_target_properties(nlohmann_json PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_subdirectory(lib)
add_subdirectory(test)