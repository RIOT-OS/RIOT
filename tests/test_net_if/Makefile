export PROJECT = test_net_if

BOARD_BLACKLIST = mbed_lpc1768 arduino-due

include ../Makefile.tests_common

ifeq ($(BOARD),stm32f4discovery)
	include Makefile.$(BOARD)
endif

USEMODULE += net_if
USEMODULE += defaulttransceiver

include $(RIOTBASE)/Makefile.include
