BOARD ?= nrf52840dk

include ../Makefile.tests_common

USEMODULE += usbus_hid

DISABLE_MODULE += auto_init_usbus

USB_VID ?= $(USB_VID_TESTING)
USB_PID ?= $(USB_PID_TESTING)

include $(RIOTBASE)/Makefile.include
