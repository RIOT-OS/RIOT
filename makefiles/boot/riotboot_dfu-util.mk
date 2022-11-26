ifneq (,$(filter usbus_dfu,$(USEMODULE)))
  ifeq (,$(filter riotboot_usb_dfu,$(USEMODULE)))
    # If module usbus_dfu is used but not module riotboot_usb_dfu, the
    # application uses DFU Runtime and dfu-util as programmer to flash the
    # application with the bootloader riotboot_dfu which uses the VID/PID pair
    # allocated for the RIOT bootloader https://pid.codes/1209/7D02/
    DFU_USB_ID ?= 1209:7d02
    DFU_USE_DFUSE ?= 0
  endif
endif
