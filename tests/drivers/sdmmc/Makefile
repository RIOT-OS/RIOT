BOARD ?= same54-xpro

include ../Makefile.drivers_common

USEMODULE += sdmmc

USEMODULE += fmt
USEMODULE += shell

OUTPUT ?= 1

CFLAGS += -DOUTPUT=$(OUTPUT)

include $(RIOTBASE)/Makefile.include
