BOARD ?= nrf52840-mdk

include ../Makefile.drivers_common

DRIVER ?= hm3301
USEMODULE += $(DRIVER)
USEMODULE += ztimer_usec
USEMODULE += ztimer_msec
USEMODULE += fmt
USEMODULE += fmt_table

include $(RIOTBASE)/Makefile.include
