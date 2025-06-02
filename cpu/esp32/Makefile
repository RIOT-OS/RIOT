# Define the module that is built:
MODULE = cpu

SRC = irq_arch.c startup.c stdatomic.c syscalls.c

# Add a list of subdirectories, that should also be built:
DIRS += $(RIOTCPU)/esp_common
DIRS += periph
DIRS += esp-idf

ifneq (, $(filter esp_bootloader, $(USEMODULE)))
    DIRS += bootloader
endif

ifneq (, $(filter usb_board_reset, $(USEMODULE)))
    SRC += usb_reset.c
endif

ifneq (, $(filter esp_ble_nimble, $(USEMODULE)))
    DIRS += esp-ble-nimble
endif

ifneq (, $(filter esp_eth, $(USEMODULE)))
    DIRS += esp-eth
endif

ifneq (, $(filter esp_freertos, $(USEMODULE)))
    DIRS += freertos
endif

ifneq (, $(filter esp_ieee802154, $(USEMODULE)))
    DIRS += esp-ieee802154
endif

ifneq (, $(filter esp_lcd, $(USEMODULE)))
    DIRS += esp-lcd
endif

ifneq (, $(filter stdio_usb_serial_jtag, $(USEMODULE)))
    DIRS += stdio_usb_serial_jtag
endif

ifneq (,$(filter esp_wifi% esp_eth, $(USEMODULE)))
    SRC += esp_ztimer.c
endif

include $(RIOTBASE)/Makefile.base
