APPLICATION = init_test
include ../Makefile.tests_common

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += xtimer

# chronos is missing a getchar implementation
BOARD_BLACKLIST += chronos

include $(RIOTBASE)/Makefile.include
