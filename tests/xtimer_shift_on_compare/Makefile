APPLICATION = xtimer_shift_on_compare
include ../Makefile.tests_common

BOARD_WHITELIST += msb-430h arduino-mega2560

BOARD ?= native
RIOTBASE ?= $(CURDIR)/../..

FEATURES_REQUIRED += periph_timer
USEMODULE += xtimer

include $(RIOTBASE)/Makefile.include
