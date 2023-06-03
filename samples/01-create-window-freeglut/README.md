CMake可以使用add_custom_command指定在target构建的不同时机执行某些命令：

// 在${target}构建完成后执行copy操作
add_custom_command(TARGET ${target} POST_BUILD
    COMMAND
        ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/shader.vs ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}${CMAKE_BUILD_TYPE}/shaders/${target}.vs
    COMMAND
        ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/shader.fs ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}${CMAKE_BUILD_TYPE}/shaders/${target}.fs
    COMMENT
        "Custom command copy shader"
)

add_custom_command 还可以使用OUTPUT关键字：

// 创建一个target，ALL表示总是构建该target
add_custom_target(copy_shader ALL 
    COMMENT
        "This is generating my custom target ${target}_copy_shader"
    DEPENDS  # 该target有依赖，依赖文件不存在时会执行下面的add_custom_command
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}${CMAKE_BUILD_TYPE}/shaders/${target}.vs"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}${CMAKE_BUILD_TYPE}/shaders/${target}.fs"
)

// 该custom_command用于生成某些文件
add_custom_command(
    OUTPUT
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}${CMAKE_BUILD_TYPE}/shaders/${target}.vs"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}${CMAKE_BUILD_TYPE}/shaders/${target}.fs"
    COMMAND
        ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${target}.vs ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}${CMAKE_BUILD_TYPE}/shaders/
    COMMAND
        ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${target}.fs ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}${CMAKE_BUILD_TYPE}/shaders/
    COMMENT
        "This is generating my custom command: ${target}"
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/${target}.vs
        ${CMAKE_CURRENT_SOURCE_DIR}/${target}.fs
)


cmake.exe提供了一些命令可以做删除，拷贝等操作，具体可以运行cmake -E查看