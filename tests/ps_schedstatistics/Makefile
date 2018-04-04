include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := chronos msb-430 msb-430h nucleo-f030 nucleo-l053 \
                             nucleo32-f031 nucleo32-f042 nucleo32-l031 \
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
