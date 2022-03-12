cmake_minimum_required(VERSION 3.21)
project(libsteam)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_SHARED_LIBRARY_PREFIX "")

add_library(libsteam SHARED
        source/api/steam_api.cpp

        source/file_formats/vdf.cpp
        source/file_formats/keyvalues.cpp

        source/helpers/file.cpp
)

set_target_properties(libsteam
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

target_link_libraries(libsteam PUBLIC fmt::fmt)

target_include_directories(libsteam PUBLIC include)

install(TARGETS libsteam)