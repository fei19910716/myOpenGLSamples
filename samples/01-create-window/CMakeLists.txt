
set(target 01-create-window)


file(GLOB src_files ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_executable(${target} ${src_files})

target_link_libraries(${target} PUBLIC base)



