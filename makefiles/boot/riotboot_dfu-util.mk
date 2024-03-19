ifneq (,$(filter usbus_dfu tinyusb_dfu,$(USEMODULE)))
  ifeq (,$(filter riotboot_usb_dfu riotboot_tinyusb_dfu,$(USEMODULE)))
    # If module usbus_dfu or module tinyusb_dfu is used but neither
    # module riotboot_usb_dfu nor module riotboot_tinyusb_dfu, the
    # application uses DFU Runtime and dfu-util as programmer to flash the
    # application with the bootloader riotboot_dfu which uses the VID/PID pair
    # allocated for the RIOT bootloader https://pid.codes/1209/7D02/
    DFU_USB_ID ?= 1209:7d02
    DFU_USE_DFUSE ?= 0
  endif
endif
