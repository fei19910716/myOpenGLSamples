

add_library(external INTERFACE)

set(external_libs
    glad
    glfw

    stb
    assimp
)

foreach(external_lib ${external_libs})
    add_subdirectory(${external_lib})
    target_link_libraries(external INTERFACE ${external_lib})
endforeach()

# 头文件的include通过lib单独的CMakeLists.txt进行，不在这一层处理
# target_include_directories(external INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})