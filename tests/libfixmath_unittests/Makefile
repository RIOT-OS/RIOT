APPLICATION = libfixmath_unittests
include ../Makefile.tests_common

BOARD_BLACKLIST := arduino-mega2560
# arduino-mega2560: builds locally but breaks travis (possibly because of
# differences in the toolchain)

# The MSP boards don't feature round(), exp(), and log(), which are used in the unittests
BOARD_INSUFFICIENT_RAM := chronos msb-430 msb-430h telosb wsn430-v1_3b wsn430-v1_4 z1

# Insufficient RAM / ROM
BOARD_INSUFFICIENT_RAM += redbee-econotag stm32f0discovery

USEMODULE += libfixmath-unittests

ifeq ($(TRAVIS),true)
  BOARD_WHITELIST := -
endif

include $(RIOTBASE)/Makefile.include
