include ../Makefile.tests_common
USEMODULE += embunit
USEMODULE += usbus
USEMODULE += usbdev_mock

DISABLE_MODULE += auto_init_usbus

# USB device vendor and product ID
USB_VID ?= $(USB_VID_TESTING)
USB_PID ?= $(USB_PID_TESTING)

include $(RIOTBASE)/Makefile.include
