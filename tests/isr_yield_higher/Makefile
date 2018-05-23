APPLICATION = isr_yield_higher
include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := nucleo-f031k6

USEMODULE += xtimer

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include

test:
	./tests/test.py
