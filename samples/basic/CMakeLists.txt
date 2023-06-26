

file(GLOB dirs ${CMAKE_CURRENT_SOURCE_DIR}/*)

# 构建所有samples
if(BUILD_ALL_SAMPLES)
    foreach(dir ${dirs})
        if(IS_DIRECTORY ${dir})
            add_subdirectory(${dir})
        endif()
    endforeach()
else()
    add_subdirectory(01-hello-window)
    add_subdirectory(02-hello-triangle)
    add_subdirectory(03-index-draw)
    add_subdirectory(04-shader-uniform)
    add_subdirectory(05-vertex-attributes)
    add_subdirectory(06-texture)
    add_subdirectory(07-model-matrix)
    add_subdirectory(08-mvp-matrix)
    # add_subdirectory(09-camera-matrix)
endif()

