

file(GLOB_RECURSE BASE_SRC "*.cpp" "*.h")

add_library(base STATIC ${BASE_SRC})

target_link_libraries(base PUBLIC external)