include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := nucleo-f031k6 nucleo-f042k6 nucleo-l031k6 nucleo-f030r8 \
                             nucleo-l053r8 stm32f0discovery

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
