include ../Makefile.tests_common

APPLICATION = log

BOARD_WHITELIST := native
USEMODULE += log_mqueue
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
TEST_ON_CI_WHITELIST += all
include $(RIOTBASE)/Makefile.include
