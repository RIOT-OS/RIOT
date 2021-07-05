add_library(kitchen_sink_libs INTERFACE)
target_sources(kitchen_sink_libs INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/kitchen_sink.c
)
target_link_libraries(kitchen_sink_libs INTERFACE
    hardware_adc
    hardware_clocks
    hardware_divider
    hardware_exception
    hardware_dma
    hardware_flash
    hardware_gpio
    hardware_i2c
    hardware_interp
    hardware_irq
    hardware_pio
    hardware_pll
    hardware_pwm
    hardware_resets
    hardware_rtc
    hardware_uart
    hardware_spi
    hardware_sync
    hardware_timer
    hardware_uart
    hardware_vreg
    hardware_watchdog
    hardware_xosc
    pico_bit_ops
    pico_bootrom
    pico_bootsel_via_double_reset
    pico_divider
    pico_double
    pico_fix_rp2040_usb_device_enumeration
    pico_float
    pico_int64_ops
    pico_malloc
    pico_mem_ops
    pico_multicore
    pico_platform
    pico_stdlib
    pico_sync
    pico_time
    pico_unique_id
    pico_util
)
# todo this is full of warnings atm
#if (TARGET tinyusb_device)
#    target_include_directories(kitchen_sink_libs INTERFACE ${CMAKE_CURRENT_LIST_DIR})
#    target_link_libraries(kitchen_sink_libs INTERFACE tinyusb_device)
#endif()

add_library(kitchen_sink_options INTERFACE)

target_compile_options(kitchen_sink_options INTERFACE
        -Werror
        -Wall
        -Wextra
        #        -pedantic

        -Wnull-dereference
        -Wuninitialized
        -Wunused
        -Wcast-align
        -Wall
        -Wcast-qual
        -Wfloat-equal
        -Wmissing-format-attribute
        -Wconversion
        -Wsign-compare
        $<$<COMPILE_LANGUAGE:C>:-Wstrict-prototypes>

        -Wno-inline
        # todo not sure these are true, but investigate
        #-Wpacked

        # todo we have some of these in usb_device_tiny to try to make it more readable.. perhaps doxygen would help here instead
        -Wredundant-decls

        -Wno-shadow
        -Wno-missing-field-initializers
        -Wno-missing-braces
        -Wno-multichar

        # todo useful but fix later
        #-Wundef
        )

target_compile_definitions(kitchen_sink_libs INTERFACE
        PARAM_ASSERTIONS_ENABLE_ALL=1 # want to check all the assertions for compilation warnings
        PICO_AUDIO_DMA_IRQ=1
)

add_executable(kitchen_sink)
target_link_libraries(kitchen_sink kitchen_sink_libs kitchen_sink_options)
pico_set_program_name(kitchen_sink "Wombat tentacles")
pico_add_extra_outputs(kitchen_sink)

add_executable(kitchen_sink_extra_stdio)
target_link_libraries(kitchen_sink_extra_stdio kitchen_sink_libs) # no kitchen_sink_options as TinyUSB has warnings
pico_add_extra_outputs(kitchen_sink_extra_stdio)
pico_enable_stdio_usb(kitchen_sink_extra_stdio 1)
pico_enable_stdio_semihosting(kitchen_sink_extra_stdio 1)

add_executable(kitchen_sink_copy_to_ram)
pico_set_binary_type(kitchen_sink_copy_to_ram copy_to_ram)
target_link_libraries(kitchen_sink_copy_to_ram kitchen_sink_libs kitchen_sink_options)
pico_add_extra_outputs(kitchen_sink_copy_to_ram)

add_executable(kitchen_sink_no_flash)
pico_set_binary_type(kitchen_sink_no_flash no_flash)
target_link_libraries(kitchen_sink_no_flash kitchen_sink_libs kitchen_sink_options)
pico_add_extra_outputs(kitchen_sink_no_flash)
