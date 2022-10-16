cmake_minimum_required(VERSION 3.17)

project(fsnotifier CXX C)

set(BUILD_EXAMPLES on)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

add_subdirectory(fsnotifier)

if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()