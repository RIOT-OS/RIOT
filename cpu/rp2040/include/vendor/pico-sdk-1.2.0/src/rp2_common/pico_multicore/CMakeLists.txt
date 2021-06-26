if (NOT TARGET pico_multicore)
    pico_add_impl_library(pico_multicore)

    target_sources(pico_multicore INTERFACE
            ${CMAKE_CURRENT_LIST_DIR}/multicore.c)

    target_include_directories(pico_multicore INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

    target_link_libraries(pico_multicore INTERFACE pico_sync hardware_irq)
endif()



