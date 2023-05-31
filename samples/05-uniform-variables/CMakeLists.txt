
set(target 05-uniform-variables)


file(GLOB src_files ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_executable(${target} ${src_files})

target_link_libraries(${target} PUBLIC base)

add_custom_target(${target}_delete_shader ALL
    COMMAND
        ${CMAKE_COMMAND} -E remove -f ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}${CMAKE_BUILD_TYPE}/shaders/${target}.vs
    COMMAND
        ${CMAKE_COMMAND} -E remove -f ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}${CMAKE_BUILD_TYPE}/shaders/${target}.fs
    COMMENT
        "This is generating my custom target ${target}_delete_shader"
)
# Here we create a custom command. ALL means this target will be always built.
add_custom_target(${target}_copy_shader ALL
    COMMENT
        "This is generating my custom target ${target}_copy_shader"
    DEPENDS
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}${CMAKE_BUILD_TYPE}/shaders/${target}.vs"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}${CMAKE_BUILD_TYPE}/shaders/${target}.fs"
)
add_dependencies(${target}_copy_shader ${target}_delete_shader)
# When copy_glut target is built, it dependends the freeglut.dll, and the following custom command will be built once.
add_custom_command(
    OUTPUT
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}${CMAKE_BUILD_TYPE}/shaders/${target}.vs"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}${CMAKE_BUILD_TYPE}/shaders/${target}.fs"
    COMMAND
        ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${target}.vs ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}${CMAKE_BUILD_TYPE}/shaders/
    COMMAND
        ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${target}.fs ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}${CMAKE_BUILD_TYPE}/shaders/
    COMMENT
        "This is generating my custom command"
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/${target}.vs
        ${CMAKE_CURRENT_SOURCE_DIR}/${target}.fs
)