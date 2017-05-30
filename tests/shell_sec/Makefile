APPLICATION = shell_sec
include ../Makefile.tests_common

USEMODULE += shell_sec_default
USEMODULE += shell_sec
USEMODULE += ps

# chronos is missing a getchar implementation
BOARD_BLACKLIST += chronos

include $(RIOTBASE)/Makefile.include
