
get_filename_component(target ${CMAKE_CURRENT_SOURCE_DIR} NAME)
message(STATUS "starting build: ${target}")

file(GLOB src_files ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_executable(${target} ${src_files})

target_link_libraries(${target} PUBLIC base)


file(GLOB shader_files
${CMAKE_CURRENT_SOURCE_DIR}/*.vs
${CMAKE_CURRENT_SOURCE_DIR}/*.fs
${CMAKE_CURRENT_SOURCE_DIR}/*.tcs
${CMAKE_CURRENT_SOURCE_DIR}/*.tes
${CMAKE_CURRENT_SOURCE_DIR}/*.gs
${CMAKE_CURRENT_SOURCE_DIR}/*.cs)


foreach(shader_file ${shader_files})
    copy_shader(${target} ${shader_file})
endforeach()