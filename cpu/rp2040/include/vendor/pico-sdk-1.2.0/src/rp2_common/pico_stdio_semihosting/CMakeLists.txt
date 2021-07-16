pico_add_impl_library(pico_stdio_semihosting)

target_sources(pico_stdio_semihosting INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/stdio_semihosting.c
)

target_include_directories(pico_stdio_semihosting INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(pico_stdio_semihosting INTERFACE pico_stdio)