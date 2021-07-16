pico_add_impl_library(pico_stdio_uart)

target_sources(pico_stdio_uart INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/stdio_uart.c
)

target_include_directories(pico_stdio_uart INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(pico_stdio_uart INTERFACE pico_stdio)