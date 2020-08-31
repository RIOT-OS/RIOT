APPLICATION = driver_onewire
include ../Makefile.tests_common

USEMODULE += onewire
USEMODULE += shell

BUS_PIN ?= GPIO_PIN\(0,0\)
CFLAGS += -DBUS_PIN=${BUS_PIN}

include $(RIOTBASE)/Makefile.include
