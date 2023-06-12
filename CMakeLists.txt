cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

project(myProject)

# set user defined module path, when use find_package to search library, the cmake will also search this path.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(settings)
include(utils)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(external)
add_subdirectory(base)
add_subdirectory(samples)

# use function to clean build
clear_build()

# use target to clean build
add_custom_target(BuildClean
    COMMAND
        ${CMAKE_COMMAND} -E rm -rf "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
    COMMENT
        "this is clean target"
)


