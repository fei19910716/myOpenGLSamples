

file(GLOB dirs ${CMAKE_CURRENT_SOURCE_DIR}/*)

foreach(dir ${dirs})
    if(IS_DIRECTORY ${dir})
        add_subdirectory(${dir})
    endif()
endforeach()



