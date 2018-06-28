include ../Makefile.tests_common

USEPKG += minmea

# The MSP-430 toolchain lacks mktime and NAN
BOARD_BLACKLIST := chronos msb-430 msb-430h telosb wsn430-v1_3b wsn430-v1_4 z1

# Incompatible due to non-standard ATmega time.h
BOARD_BLACKLIST += arduino-duemilanove \
                   arduino-mega2560 \
                   arduino-uno \
                   jiminy-mega256rfr2 \
                   mega-xplained \
                   waspmote-pro

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
