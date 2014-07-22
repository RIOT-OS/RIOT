APPLICATION = libfixmath_unittests
include ../Makefile.tests_common

# The MSP boards don't feature round(), exp(), and log(), which are used in the unittests
BOARD_INSUFFICIENT_RAM := chronos msb-430 msb-430h telosb wsn430-v1_3b wsn430-v1_4 z1

# Insufficient RAM / ROM
BOARD_INSUFFICIENT_RAM += redbee-econotag stm32f0discovery

USEMODULE += libfixmath-unittests

include $(RIOTBASE)/Makefile.include
