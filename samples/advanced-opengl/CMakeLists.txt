

file(GLOB dirs ${CMAKE_CURRENT_SOURCE_DIR}/*)

# 构建所有samples
if(BUILD_ALL_SAMPLES)
    foreach(dir ${dirs})
        if(IS_DIRECTORY ${dir})
            add_subdirectory(${dir})
        endif()
    endforeach()
else()
    add_subdirectory(00-load-model)
    add_subdirectory(01-depth-view)
    add_subdirectory(02-stencil-test)
    add_subdirectory(03-alpha-test)
    add_subdirectory(04-blending)
    add_subdirectory(05-fbo)
endif()

