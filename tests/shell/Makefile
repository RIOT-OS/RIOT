APPLICATION = shell
include ../Makefile.tests_common

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

DISABLE_MODULE += auto_init

# chronos is missing a getchar implementation
BOARD_BLACKLIST += chronos

include $(RIOTBASE)/Makefile.include
