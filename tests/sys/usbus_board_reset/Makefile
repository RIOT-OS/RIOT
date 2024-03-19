include ../Makefile.sys_common

FEATURES_REQUIRED += periph_usbdev

USEMODULE += app_metadata
USEMODULE += ps
USEMODULE += shell_cmds_default
USEMODULE += usb_board_reset
USEMODULE += usbus_cdc_acm

DISABLE_MODULE += auto_init_usbus

USB_VID ?= $(USB_VID_TESTING)
USB_PID ?= $(USB_PID_TESTING)

include $(RIOTBASE)/Makefile.include
