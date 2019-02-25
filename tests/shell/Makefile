DEVELHELP=0
include ../Makefile.tests_common

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

DISABLE_MODULE += auto_init

# chronos is missing a getchar implementation
BOARD_BLACKLIST += chronos

TEST_ON_CI_WHITELIST += all

# In order to properly test and debug the shell it is better to use a terminal
# program that does not modify the input and output streams and has no buffering.
ifneq ($(BOARD), native)
  RIOT_TERMINAL ?= miniterm.py
endif

include $(RIOTBASE)/Makefile.include
