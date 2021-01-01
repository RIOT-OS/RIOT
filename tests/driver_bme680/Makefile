include ../Makefile.tests_common

BOARD ?= bluepill-128kib
PORT = /dev/ttyUSB0

DRIVER ?= bme680_ic2

USEMODULE += $(DRIVER)
USEMODULE += xtimer

include $(RIOTBASE)/Makefile.include
