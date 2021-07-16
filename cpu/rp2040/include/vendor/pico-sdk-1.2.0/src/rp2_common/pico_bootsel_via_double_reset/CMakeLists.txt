pico_add_impl_library(pico_bootsel_via_double_reset)

target_sources(pico_bootsel_via_double_reset INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/pico_bootsel_via_double_reset.c
        )

target_link_libraries(pico_bootsel_via_double_reset INTERFACE
        pico_bootrom
        pico_time
        )
