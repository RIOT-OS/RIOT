BOARD ?= samr21-xpro
include ../Makefile.pkg_common

USB_VID ?= $(USB_VID_TESTING)
USB_PID ?= $(USB_PID_TESTING)

USEMODULE += stdio_tinyusb_cdc_acm

USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += ps

include $(RIOTBASE)/Makefile.include
