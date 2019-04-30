DEVELHELP=0
include ../Makefile.tests_common

USEMODULE += app_metadata
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

DISABLE_MODULE += auto_init

# chronos is missing a getchar implementation
BOARD_BLACKLIST += chronos

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove \
                             arduino-nano \
                             arduino-uno

TEST_ON_CI_WHITELIST += all

APP_SHELL_FMT ?= NONE

include $(RIOTBASE)/Makefile.include
