DEVELHELP=0
include ../Makefile.tests_common

USEMODULE += shell
USEMODULE += shell_commands

USEMODULE += shell_lock
USEMODULE += shell_lock_auto_locking

CFLAGS += -DSHELL_LOCK_PASSWORD=\"password\"
CFLAGS += -DSHELL_LOCK_AUTO_LOCK_TIMEOUT_MS=7000

# for z1, socat doesn't work (unknown reason)
ifeq (z1, $(BOARD))
  RIOT_TERMINAL ?= pyterm
endif

# Use a terminal that does not introduce extra characters into the stream.
RIOT_TERMINAL ?= socat

include $(RIOTBASE)/Makefile.include

# the test script skips tests if socat is not used
$(call target-export-variables,$(RIOT_TERMINAL),RIOT_TERMINAL)
