BOARD ?= adafruit-feather-nrf52840-sense
include ../Makefile.drivers_common

DISABLE_MODULE += test_utils_interactive_sync

DRIVER ?= lsm6dsxx

USEMODULE += $(DRIVER)
USEMODULE += ztimer
USEMODULE += ztimer_msec

include $(RIOTBASE)/Makefile.include
