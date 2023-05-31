cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

project(myOgldev)

# set user defined module path, when use find_package to search library, the cmake will also search this path.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(external)
add_subdirectory(base)
add_subdirectory(samples)

message(STATUS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}) 
add_custom_target(BuildClean
    COMMAND
        cmd rmdir /s /q ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMENT
        "this is clean target"
)


