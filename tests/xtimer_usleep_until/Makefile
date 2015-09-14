APPLICATION = xtimer_sleep_until
include ../Makefile.tests_common

BOARD ?= native
RIOTBASE ?= $(CURDIR)/../..

BOARD_INSUFFICIENT_MEMORY := chronos

FEATURES_REQUIRED += periph_timer
USEMODULE += xtimer

CFLAGS += -DNUMOF=200

include $(RIOTBASE)/Makefile.include
