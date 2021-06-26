if (TARGET tinyusb_device_unmarked)
    pico_add_impl_library(pico_stdio_usb)

    target_include_directories(pico_stdio_usb INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

    target_sources(pico_stdio_usb INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/reset_interface.c
        ${CMAKE_CURRENT_LIST_DIR}/stdio_usb.c
        ${CMAKE_CURRENT_LIST_DIR}/stdio_usb_descriptors.c
    )

    target_link_libraries(pico_stdio_usb INTERFACE
        tinyusb_device_unmarked
        pico_stdio
        pico_time
        pico_unique_id
    )
endif()
