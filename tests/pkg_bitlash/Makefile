APPLICATION = testbitlash
include ../Makefile.tests_common

#FEATURES_REQUIRED = periph_random
USEPKG += bitlash

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += uart0

DISABLE_MODULE += auto_init

# chronos is missing a getchar implementation
BOARD_BLACKLIST += chronos

include $(RIOTBASE)/Makefile.include
