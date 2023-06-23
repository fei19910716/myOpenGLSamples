get_filename_component(target ${CMAKE_CURRENT_SOURCE_DIR} NAME)

add_target(${target}_gl ${CMAKE_CURRENT_SOURCE_DIR}/gl)

add_target(${target}_vk ${CMAKE_CURRENT_SOURCE_DIR}/vk)