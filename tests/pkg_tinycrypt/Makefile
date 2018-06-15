include ../Makefile.tests_common

# tinycrypt works for 32-bit architectures only. The nrf52dk is chosen as a
# placeholder for all Cortex-M4 boards.
BOARD_WHITELIST += native nrf52dk

TEST_ON_CI_WHITELIST += all

USEPKG += tinycrypt
USEMODULE = fmt

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
