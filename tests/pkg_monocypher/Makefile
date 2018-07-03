include ../Makefile.tests_common

# No 8 bit and 16 bit support
BOARD_BLACKLIST := arduino-duemilanove arduino-mega2560 arduino-uno chronos \
        jiminy-mega256rfr2 mega-xplained msb-430 msb-430h telosb \
        waspmote-pro wsn430-v1_3b wsn430-v1_4 z1

USEMODULE += embunit
USEMODULE += random
USEPKG += monocypher

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
