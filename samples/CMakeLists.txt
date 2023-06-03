

file(GLOB dirs ${CMAKE_CURRENT_SOURCE_DIR}/*)

# 构建所有samples
# foreach(dir ${dirs})
#     if(IS_DIRECTORY ${dir})
#         add_subdirectory(${dir})
#     endif()
# endforeach()

add_subdirectory(01-hello-window-glfw)
add_subdirectory(14-camera-key-control)
add_subdirectory(15-camera-mouse-control)