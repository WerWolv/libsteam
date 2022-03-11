cmake_minimum_required(VERSION 3.21)
project(main)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_SHARED_LIBRARY_PREFIX "")

add_subdirectory(external/fmt)
set_target_properties(fmt PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_subdirectory(lib)
add_subdirectory(test)