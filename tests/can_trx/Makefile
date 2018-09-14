export APPLICATION = can_trx
include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-uno

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

USEMODULE += can_trx

TRX_TO_BUILD ?= tja1042

ifneq (,$(filter tja1042,$(TRX_TO_BUILD)))
    USEMODULE += tja1042
endif

include $(TESTBASE)/../Makefile.include
