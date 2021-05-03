include ../Makefile.tests_common

USEMODULE += dbgpin
USEMODULE += xtimer

# Default port used is PORT 1
# - frdm-64f port A0-4 are connected to the JTAG, setting GPIO(0,0) will
#   cause flashing to fail
BOARDS_DEBUG_PORT_1 = \
    frdm-k64f \
    #

ifneq (,$(filter $(BOARD),$(BOARDS_DEBUG_PORT_1)))
  TEST_PORT ?= 1
else
  TEST_PORT ?= 0
endif

DBGPIN_PINS ?= GPIO_PIN($(TEST_PORT),0)
CFLAGS += -DDBGPIN_PINS="$(DBGPIN_PINS)"

include $(RIOTBASE)/Makefile.include
