
get_filename_component(target ${CMAKE_CURRENT_SOURCE_DIR} NAME)

add_target(${target}_gl ${CMAKE_CURRENT_SOURCE_DIR}/gl)

add_target(${target}_vk ${CMAKE_CURRENT_SOURCE_DIR}/vk)
if(WIN32)
target_compile_definitions(${target}_vk PUBLIC VK_USE_PLATFORM_WIN32_KHR)
endif()
