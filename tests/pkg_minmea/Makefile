include ../Makefile.tests_common

USEPKG += minmea

# The MSP-430 toolchain lacks mktime and NAN
BOARD_BLACKLIST := chronos msb-430 msb-430h telosb wsn430-v1_3b wsn430-v1_4 z1

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
