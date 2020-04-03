include ../Makefile.tests_common

USEMODULE += shell
USEMODULE += timex

# The MSP-430 toolchain lacks sscanf:
BOARD_BLACKLIST := chronos msb-430 msb-430h telosb wsn430-v1_3b wsn430-v1_4 z1

include $(RIOTBASE)/Makefile.include
