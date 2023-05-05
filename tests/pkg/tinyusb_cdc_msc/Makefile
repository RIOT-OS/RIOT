BOARD ?= samr21-xpro
include ../Makefile.pkg_common

USB_VID ?= $(USB_VID_TESTING)
USB_PID ?= $(USB_PID_TESTING)

USEPKG += tinyusb
USEMODULE += tinyusb_class_cdc
USEMODULE += tinyusb_class_msc
USEMODULE += tinyusb_device

include $(RIOTBASE)/Makefile.include
