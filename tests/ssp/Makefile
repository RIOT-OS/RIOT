include ../Makefile.tests_common

# avr8, msp430, esp8266 and mips don't support ssp (yet)
BOARD_BLACKLIST := arduino-mega2560 waspmote-pro arduino-uno arduino-duemilanove \
                   chronos msb-430 msb-430h telosb wsn430-v1_3b wsn430-v1_4 z1 \
                   pic32-clicker pic32-wifire jiminy-mega256rfr2 mega-xplained \
                   esp8266-esp-12x esp8266-olimex-mod esp8266-sparkfun-thing

USEMODULE += ssp

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
