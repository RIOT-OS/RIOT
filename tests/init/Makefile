APPLICATION = init_test

BOARD = pba-d-01-kw2x
#BOARD = samr21-xpro
#BOARD = msb-430
#BOARD = arduino-mega2560

QUIET = 0

include ../Makefile.tests_common

USEMODULE += preinit
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += xtimer

# chronos is missing a getchar implementation
BOARD_BLACKLIST += chronos

include $(RIOTBASE)/Makefile.include
