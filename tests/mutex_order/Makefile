include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := nucleo32-f031 nucleo32-f042 nucleo32-l031 nucleo-f030 \
                             nucleo-l053 stm32f0discovery

# list of boards to run CI tests on
TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
