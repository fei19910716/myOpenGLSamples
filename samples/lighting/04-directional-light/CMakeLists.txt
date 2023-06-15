
get_filename_component(target ${CMAKE_CURRENT_SOURCE_DIR} NAME)
message(STATUS "starting build: ${target}")

file(GLOB src_files ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_executable(${target} ${src_files})

target_link_libraries(${target} PUBLIC base)

add_custom_command(TARGET ${target} POST_BUILD
    COMMAND
        ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/5.1.lighting.vs   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}${CMAKE_BUILD_TYPE}/shaders/5.1.lighting.vs
    COMMAND
        ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/5.1.lighting.fs   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}${CMAKE_BUILD_TYPE}/shaders/5.1.lighting.fs
    COMMAND
        ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/5.1.light_cube.vs ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}${CMAKE_BUILD_TYPE}/shaders/5.1.light_cube.vs
    COMMAND
        ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/5.1.light_cube.fs ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}${CMAKE_BUILD_TYPE}/shaders/5.1.light_cube.fs
    COMMENT
        "Custom command copy shader"
)

