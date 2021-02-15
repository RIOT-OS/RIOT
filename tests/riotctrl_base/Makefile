include ../Makefile.tests_common

USEMODULE += app_metadata
USEMODULE += ps
USEMODULE += shell
USEMODULE += shell_commands

# Use a terminal that does not introduce extra characters into the stream.
RIOT_TERMINAL ?= socat

include $(RIOTBASE)/Makefile.include

ifndef CONFIG_SHELL_NO_ECHO
  CFLAGS += -DCONFIG_SHELL_NO_ECHO=1
endif
