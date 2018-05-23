include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := chronos msb-430 msb-430h nucleo-f030r8 nucleo-l053r8 \
                             nucleo-f031k6 nucleo-f042k6 nucleo-l031k6 \
                             stm32f0discovery telosb wsn430-v1_3b \
                             wsn430-v1_4 z1

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += schedstatistics
USEMODULE += printf_float

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
