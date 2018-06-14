include ../Makefile.tests_common

# MIPS based boards are black listed due to some issue with newlib and _Atomic
BOARD_BLACKLIST := mips-malta pic32-clicker pic32-wifire

BOARD_INSUFFICIENT_MEMORY := nucleo-f031k6 nucleo-f042k6 nucleo-l031k6 nucleo-f030r8 \
                             nucleo-l053r8 stm32f0discovery

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
