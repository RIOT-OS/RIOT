APPLICATION = net_if

include ../Makefile.tests_common

ifeq ($(BOARD),stm32f4discovery)
	include Makefile.$(BOARD)
endif

USEMODULE += net_if
USEMODULE += defaulttransceiver
USEMODULE += msg_queue

include $(RIOTBASE)/Makefile.include
