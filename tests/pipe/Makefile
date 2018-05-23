include ../Makefile.tests_common

#malloc.h not found
BOARD_BLACKLIST := jiminy-mega256rfr2 mega-xplained
BOARD_INSUFFICIENT_MEMORY := nucleo-f031k6

USEMODULE += pipe

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
