APPLICATION = test_net_if

BOARD_BLACKLIST = mbed_lpc1768 arduino-due udoo qemu-i386 stm32f0discovery stm32f4discovery
# qemu-i386:        no transceiver, yet
# stm32f0discovery: no transceiver, yet
# stm32f4discovery: no transceiver, yet

include ../Makefile.tests_common

ifeq ($(BOARD),stm32f4discovery)
	include Makefile.$(BOARD)
endif

USEMODULE += net_if
USEMODULE += defaulttransceiver

include $(RIOTBASE)/Makefile.include
