

add_library(external INTERFACE)

set(external_libs
    glut
    glew
)

foreach(external_lib ${external_libs})
    add_subdirectory(${external_lib})
    target_link_libraries(external INTERFACE ${external_lib})
endforeach()


# target_include_directories(external INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})