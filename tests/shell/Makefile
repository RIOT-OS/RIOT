DEVELHELP=0
include ../Makefile.tests_common

USEMODULE += app_metadata
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# for z1, socat doesn't work (unknown reason)
ifeq (z1, $(BOARD))
  RIOT_TERMINAL ?= pyterm
endif

# Use a terminal that does not introduce extra characters into the stream.
RIOT_TERMINAL ?= socat

APP_SHELL_FMT ?= NONE

include $(RIOTBASE)/Makefile.include

# the test script skips tests if socat is not used
$(call target-export-variables,$(RIOT_TERMINAL),RIOT_TERMINAL)
