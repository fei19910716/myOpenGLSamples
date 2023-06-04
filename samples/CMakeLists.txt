

file(GLOB dirs ${CMAKE_CURRENT_SOURCE_DIR}/*)

# 构建所有samples
if(BUILD_ALL_SAMPLES)
    foreach(dir ${dirs})
        if(IS_DIRECTORY ${dir})
            add_subdirectory(${dir})
        endif()
    endforeach()
else()
    # add_subdirectory(01-hello-window-glfw)
    # add_subdirectory(02-hello-dot-fix-pipeline)
    # add_subdirectory(04-hello-triangle)
    # add_subdirectory(14-camera-key-control)
    # add_subdirectory(15-camera-mouse-control)
    # add_subdirectory(16-texture)
    add_subdirectory(lighting)
endif()

