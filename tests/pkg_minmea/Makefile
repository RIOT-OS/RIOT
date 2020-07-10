include ../Makefile.tests_common

USEPKG += minmea

USEMODULE += fmt

# The MSP-430 toolchain lacks mktime and NAN
BOARD_BLACKLIST := chronos msb-430 msb-430h telosb z1

include $(RIOTBASE)/Makefile.include
