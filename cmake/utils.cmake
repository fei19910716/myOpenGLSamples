function(clear_build)
    if(IS_DIRECTORY ${CMAKE_BINARY_DIR})
        # message(STATUS "${CMAKE_BINARY_DIR} is dir")
        file(GLOB dir_to_rm_list ${CMAKE_BINARY_DIR}/*)

        foreach(dir ${dir_to_rm_list})
            if(IS_DIRECTORY ${dir})
                # message(STATUS "${dir} is directory")
                file(REMOVE_RECURSE ${dir})
            else()
                # message(STATUS "${dir} is FILE")
                file(REMOVE ${dir})
            endif()
        
        endforeach()

    endif()
endfunction()


function(copy_shader target file)
get_filename_component(file_name ${file} NAME)
add_custom_command(TARGET ${target} POST_BUILD
COMMAND
    ${CMAKE_COMMAND} -E copy_if_different ${file}  $<TARGET_FILE_DIR:${target}>/shaders/${file_name}
COMMENT
    "Custom command copy shader"
)
endfunction(copy_shader target file)


function(copy_dll target file)
get_filename_component(file_name ${file} NAME)
add_custom_target(copy_${target} ALL
    DEPENDS
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}${CMAKE_BUILD_TYPE}/${file_name}"
)

# When copy_glfw target is built, it dependends the freeglut.dll, and the following custom command will be built once.
add_custom_command(
    OUTPUT
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}${CMAKE_BUILD_TYPE}/${file_name}"
    COMMAND
        ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/lib/${file_name} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}${CMAKE_BUILD_TYPE}
    COMMENT
        "This is generating my custom command"
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/${file_name}
)

endfunction(copy_dll target file)