include ../Makefile.sys_common

USEMODULE += shell
USEMODULE += shell_cmd_coreclk

# Use a terminal that does not introduce extra characters into the stream.
RIOT_TERMINAL ?= socat

include $(RIOTBASE)/Makefile.include

# the test script skips tests if socat is not used
$(call target-export-variables,$(RIOT_TERMINAL),RIOT_TERMINAL)
