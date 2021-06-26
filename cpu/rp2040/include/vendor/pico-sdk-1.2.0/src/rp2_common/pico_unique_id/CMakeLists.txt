pico_add_impl_library(pico_unique_id)

target_sources(pico_unique_id INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/unique_id.c
)

target_include_directories(pico_unique_id INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(pico_unique_id INTERFACE hardware_flash)
