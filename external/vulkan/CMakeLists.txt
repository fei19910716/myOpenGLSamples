get_filename_component(target ${CMAKE_CURRENT_SOURCE_DIR} NAME)
message(STATUS "starting build: ${target}")

add_library(${target} INTERFACE)

# since version 3.7.0, the cmake will offer a module named FindVulkan.cmake under the install dir, which will search the vulkan library under the {VULKAN_SDK} path.
if(NOT CMAKE_VERSION VERSION_LESS 3.7.0)
    message(STATUS "Using module to find Vulkan")
    find_package(Vulkan)
endif()

IF(WIN32)
    IF(NOT Vulkan_FOUND)
        # use find_library to search the library under user defined path.
        # find_library(Vulkan_LIBRARY NAMES vulkan-1 vulkan PATHS ${CMAKE_CURRENT_SOURCE_DIR}/vulkan/lib)
        MESSAGE("-- Using bundled Vulkan library version")
        file(GLOB lib_files ${CMAKE_CURRENT_SOURCE_DIR}/lib/*.lib)

        target_link_libraries(${target} INTERFACE ${lib_files})
    elseif()

        target_link_libraries(${target} INTERFACE ${Vulkan_LIBRARY})

    endif()

    target_include_directories(${target} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

    # vulkan headers are platform related, we use WIN32 platfrom here.
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-DVK_USE_PLATFORM_WIN32_KHR")

endif()


file(GLOB dll_files ${CMAKE_CURRENT_SOURCE_DIR}/lib/*.dll)
foreach(dll_file ${dll_files})
    copy_dll(${target} ${dll_file})
endforeach(dll_file ${dll_files})