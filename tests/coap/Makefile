APPLICATION = coap
include ../Makefile.tests_common

# the MSP430 toolchain does not include assert.h
BOARD_BLACKLIST := chronos msb-430 msb-430h telosb wsn430-v1_3b wsn430-v1_4 z1

BOARD_INSUFFICIENT_RAM := redbee-econotag

USEMODULE += defaulttransceiver
USEPKG += libcoap

include $(RIOTBASE)/Makefile.include
