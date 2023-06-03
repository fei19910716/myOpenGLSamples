

# add_library(glut SHARED IMPORTED)
# set_property(TARGET glut PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/freeglut.dll)
# set_property(TARGET glut PROPERTY IMPORTED_IMPLIB   ${CMAKE_CURRENT_SOURCE_DIR}/lib/freeglut.lib) # 多了lib信息
# set_property(TARGET glut PROPERTY PUBLIC_HEADER     ${CMAKE_CURRENT_SOURCE_DIR}/include) # 多了lib信

add_library(glfw INTERFACE)

file(GLOB lib_files ${CMAKE_CURRENT_SOURCE_DIR}/lib/*.lib)

target_link_libraries(glfw INTERFACE ${lib_files})

target_include_directories(glfw INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)