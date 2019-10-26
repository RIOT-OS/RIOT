DEVELHELP=0
include ../Makefile.tests_common

USEMODULE += app_metadata
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# Use a terminal that does not introduce extra characters into the stream.
RIOT_TERMINAL ?= socat

DISABLE_MODULE += auto_init

APP_SHELL_FMT ?= NONE

# chronos is missing a getchar implementation
BOARD_BLACKLIST += \
  chronos \
  #

include $(RIOTBASE)/Makefile.include
