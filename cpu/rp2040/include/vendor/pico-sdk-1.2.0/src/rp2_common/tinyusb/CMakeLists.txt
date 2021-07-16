if (DEFINED ENV{PICO_TINYUSB_PATH} AND (NOT PICO_TINYUSB_PATH))
    set(PICO_TINYUSB_PATH $ENV{PICO_TINYUSB_PATH})
    message("Using PICO_TINYUSB_PATH from environment ('${PICO_TINYUSB_PATH}')")
endif ()

set(TINYUSB_TEST_PATH "src/portable/raspberrypi/rp2040")
if (NOT PICO_TINYUSB_PATH)
    set(PICO_TINYUSB_PATH ${PROJECT_SOURCE_DIR}/lib/tinyusb)
    if (NOT EXISTS ${PICO_TINYUSB_PATH}/${TINYUSB_TEST_PATH})
        message(WARNING "TinyUSB submodule has not been initialized; USB support will be unavailable
hint: try 'git submodule update --init' from your SDK directory (${PICO_SDK_PATH}).")
    endif()
elseif (NOT EXISTS ${PICO_TINYUSB_PATH}/${TINYUSB_TEST_PATH})
    message(WARNING "PICO_TINYUSB_PATH specified but content not present.")
endif()

if (EXISTS ${PICO_TINYUSB_PATH}/${TINYUSB_TEST_PATH})
    message("TinyUSB available at ${PICO_TINYUSB_PATH}/${TINYUSB_TEST_PATH}; adding USB support.")

    pico_register_common_scope_var(PICO_TINYUSB_PATH)

    set(BOARD pico_sdk)
    include(${PICO_TINYUSB_PATH}/hw/bsp/rp2040/family.cmake)

    add_library(tinyusb_common INTERFACE)
    target_link_libraries(tinyusb_common INTERFACE tinyusb_common_base)

    add_library(tinyusb_device_unmarked INTERFACE)
    target_link_libraries(tinyusb_device_unmarked INTERFACE tinyusb_device_base)
    target_compile_definitions(tinyusb_device_unmarked INTERFACE
            # off by default note TUD_OPT_RP2040_USB_DEVICE_ENUMERATION_FIX defaults from PICO_RP2040_USB_DEVICE_ENUMERATION_FIX
#            TUD_OPT_RP2040_USB_DEVICE_ENUMERATION_FIX=1
    )

    # unmarked version used by stdio USB
    target_link_libraries(tinyusb_device_unmarked INTERFACE tinyusb_common pico_fix_rp2040_usb_device_enumeration tinyusb_device_base)

    pico_add_impl_library(tinyusb_device)
    target_link_libraries(tinyusb_device INTERFACE tinyusb_device_unmarked)

    pico_add_impl_library(tinyusb_host)
    target_link_libraries(tinyusb_host INTERFACE tinyusb_host_base tinyusb_common)

    pico_add_impl_library(tinyusb_board)
    target_link_libraries(tinyusb_board INTERFACE tinyusb_bsp)

    pico_promote_common_scope_vars()
endif()
