DEVELHELP=0

include ../Makefile.tests_common

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

DISABLE_MODULE += auto_init

# chronos is missing a getchar implementation
BOARD_BLACKLIST += chronos

TEST_ON_CI_WHITELIST += all

ifneq ($(BOARD), native)
	TERMPROG=$(abspath miniterm.sh)
endif

include $(RIOTBASE)/Makefile.include
