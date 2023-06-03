
file(GLOB lib_files ${CMAKE_CURRENT_SOURCE_DIR}/*.c)

add_library(glad ${lib_files})

target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)