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
