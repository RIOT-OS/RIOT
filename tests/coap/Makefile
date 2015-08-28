APPLICATION = coap
include ../Makefile.tests_common

# the MSP430 and avr8 toolchains do not include assert.h
BOARD_BLACKLIST := arduino-mega2560 chronos msb-430 msb-430h telosb \
                   wsn430-v1_3b wsn430-v1_4 z1

BOARD_INSUFFICIENT_MEMORY := redbee-econotag

USEPKG += libcoap

include $(RIOTBASE)/Makefile.include
