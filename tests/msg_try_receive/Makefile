include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := nucleo-f031k6

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
