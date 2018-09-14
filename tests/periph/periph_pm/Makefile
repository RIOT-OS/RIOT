BOARD ?= slstk3401a
include ../Makefile.tests_common

# method `fflush()` is not defined for MSP-430 (#6445 will fix this)
BOARD_BLACKLIST := chronos msb-430h msb-430 telosb wsn430-v1_3b wsn430-v1_4 z1

FEATURES_OPTIONAL += periph_rtc

USEMODULE += shell

include $(RIOTBASE)/Makefile.include
