include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove

# msp430-libc does not provide `stdout` variable required by ubasic_tests
# avr-libc does not provide `clock `required by ubasic_tests
# newlib for esp8266 and MIPS does not provide _times_r
BOARD_BLACKLIST := \
                   arduino-duemilanove \
                   arduino-mega2560 \
                   arduino-nano \
                   arduino-uno \
                   chronos \
                   esp8266-esp-12x \
                   esp8266-olimex-mod \
                   hifive1 \
                   jiminy-mega256rfr2 \
                   mega-xplained \
                   mips-malta \
                   msb-430 \
                   msb-430h \
                   pic32-clicker \
                   pic32-wifire \
                   telosb \
                   waspmote-pro \
                   wsn430-v1_3b \
                   wsn430-v1_4 \
                   z1 \
                   #

TEST_ON_CI_WHITELIST += all

USEPKG += ubasic
USEMODULE += ubasic_tests

include $(RIOTBASE)/Makefile.include
