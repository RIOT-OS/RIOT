include ../Makefile.tests_common

# avr8, msp430, esp8266 and mips don't support ssp (yet)
BOARD_BLACKLIST := arduino-duemilanove \
                   arduino-mega2560 \
                   arduino-uno \
                   chronos \
                   esp8266-esp-12x \
                   esp8266-olimex-mod \
                   esp8266-sparkfun-thing \
                   jiminy-mega256rfr2 \
                   mega-xplained \
                   msb-430 \
                   msb-430h \
                   pic32-clicker \
                   pic32-wifire \
                   telosb \
                   waspmote-pro \
                   wsn430-v1_3b \
                   wsn430-v1_4 \
                   z1

USEMODULE += ssp

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include
