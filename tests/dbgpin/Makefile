include ../Makefile.tests_common

USEMODULE += dbgpin
USEMODULE += xtimer

DBGPIN_PINS ?= GPIO_PIN(0,0)
CFLAGS += -DDBGPIN_PINS="$(DBGPIN_PINS)"

include $(RIOTBASE)/Makefile.include
