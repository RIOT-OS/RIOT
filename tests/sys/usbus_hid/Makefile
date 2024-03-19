BOARD ?= nrf52840dk

include ../Makefile.sys_common

USEMODULE += usbus_hid
USEMODULE += xtimer

DISABLE_MODULE += auto_init_usbus

USB_VID ?= $(USB_VID_TESTING)
USB_PID ?= $(USB_PID_TESTING)

include $(RIOTBASE)/Makefile.include
