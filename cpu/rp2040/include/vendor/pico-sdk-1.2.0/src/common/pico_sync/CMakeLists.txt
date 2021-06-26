if (NOT TARGET pico_sync_headers)
    add_library(pico_sync_headers INTERFACE)
    target_include_directories(pico_sync_headers INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
    target_link_libraries(pico_sync_headers INTERFACE hardware_sync pico_time)
endif()

if (NOT TARGET pico_sync_core)
    pico_add_impl_library(pico_sync_core)
    target_sources(pico_sync_core INTERFACE
            ${CMAKE_CURRENT_LIST_DIR}/lock_core.c
    )
    target_link_libraries(pico_sync_core INTERFACE pico_sync_headers)
endif()

if (NOT TARGET pico_sync_sem)
    pico_add_impl_library(pico_sync_sem)
    target_sources(pico_sync_sem INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/sem.c
    )
    target_link_libraries(pico_sync_sem INTERFACE pico_sync_core pico_time)
endif()

if (NOT TARGET pico_sync_mutex)
    pico_add_impl_library(pico_sync_mutex)
    target_sources(pico_sync_mutex INTERFACE
            ${CMAKE_CURRENT_LIST_DIR}/mutex.c
            )
    target_link_libraries(pico_sync_mutex INTERFACE pico_sync_core pico_time)
endif()

if (NOT TARGET pico_sync_critical_section)
    pico_add_impl_library(pico_sync_critical_section)
    target_sources(pico_sync_critical_section INTERFACE
            ${CMAKE_CURRENT_LIST_DIR}/critical_section.c
            )
    target_link_libraries(pico_sync_critical_section INTERFACE pico_sync_core pico_time)
endif()

if (NOT TARGET pico_sync)
    pico_add_impl_library(pico_sync)
    target_link_libraries(pico_sync INTERFACE pico_sync_sem pico_sync_mutex pico_sync_critical_section pico_sync_core)
endif()


