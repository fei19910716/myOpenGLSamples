

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")


option(BUILD_ALL_SAMPLES "Build all samples" OFF)

configure_file(${CMAKE_SOURCE_DIR}/configuration/root_directory.h.in ${CMAKE_SOURCE_DIR}/configuration/root_directory.h)
include_directories(${CMAKE_SOURCE_DIR}/configuration)