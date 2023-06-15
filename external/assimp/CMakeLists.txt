
get_filename_component(target ${CMAKE_CURRENT_SOURCE_DIR} NAME)
message(STATUS "starting build: ${target}")

add_library(${target} INTERFACE)

file(GLOB lib_files ${CMAKE_CURRENT_SOURCE_DIR}/lib/*.lib)

target_link_libraries(${target} INTERFACE ${lib_files})

target_include_directories(${target} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)


file(GLOB dll_files ${CMAKE_CURRENT_SOURCE_DIR}/lib/*.dll)
foreach(dll_file ${dll_files})
    copy_dll(${target} ${dll_file})
endforeach(dll_file ${dll_files})


# add_library(glut SHARED IMPORTED)
# set_property(TARGET glut PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/freeglut.dll)
# set_property(TARGET glut PROPERTY IMPORTED_IMPLIB   ${CMAKE_CURRENT_SOURCE_DIR}/lib/freeglut.lib) # 多了lib信息
# set_property(TARGET glut PROPERTY PUBLIC_HEADER     ${CMAKE_CURRENT_SOURCE_DIR}/include) # 多了lib信


# Here we create a custom command. ALL means this target will be always built.
# add_custom_target(copy_assimp ALL
#     DEPENDS
#         "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}${CMAKE_BUILD_TYPE}/assimp-vc142-mt.dll"
# )

# # When copy_glut target is built, it dependends the freeglut.dll, and the following custom command will be built once.
# add_custom_command(
#     OUTPUT
#         "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}${CMAKE_BUILD_TYPE}/assimp-vc142-mt.dll"
#     COMMAND
#         ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp-vc142-mt.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}${CMAKE_BUILD_TYPE}
#     COMMENT
#         "This is generating my custom command"
#     DEPENDS
#         ${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp-vc142-mt.dll
# )


# Here target "glut" is an INTERFACE library that may not have PRE_BUILD, PRE_LINK, or POST_BUILD commands.
# So we cannot use the following custom command to do copy.
# add_custom_command(
#     TARGET 
#         glut POST_BUILD
#     COMMAND 
#         ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/lib/freeglut.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}${CMAKE_BUILD_TYPE}
# )