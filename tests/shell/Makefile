DEVELHELP=0
include ../Makefile.tests_common

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

DISABLE_MODULE += auto_init

TEST_ON_CI_WHITELIST += all

# chronos is missing a getchar implementation
BOARD_BLACKLIST := \
  chronos \
  #

include $(RIOTBASE)/Makefile.include
