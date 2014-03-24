export PROJECT = test_net_if

BOARD_BLACKLIST = mbed_lpc1768

include ../Makefile.tests_common

ifeq ($(BOARD),stm32f4discovery)
	include Makefile.$(BOARD)
endif

USEMODULE += auto_init
USEMODULE += net_if
USEMODULE += defaulttransceiver

export INCLUDES += -I$(RIOTBASE)/sys/net/include

include $(RIOTBASE)/Makefile.include
