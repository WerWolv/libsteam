cmake_minimum_required(VERSION 3.21)
project(test_app)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_SHARED_LIBRARY_PREFIX "")

add_executable(test_app
        source/main.cpp
        )

target_link_libraries(test_app PUBLIC libsteam)

set_target_properties(test_app
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        )

install(TARGETS test_app)