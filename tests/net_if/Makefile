APPLICATION = net_if

BOARD_BLACKLIST = mbed_lpc1768 arduino-due udoo qemu-i386 stm32f0discovery stm32f3discovery \
                  stm32f4discovery pca10000 pca10005 arduino-mega2560
# qemu-i386:        no transceiver, yet
# stm32f0discovery: no transceiver, yet
# stm32f3discovery: no transceiver, yet
# stm32f4discovery: no transceiver, yet
# pca10000:         no transceiver, yet
# pca10005:         no transceiver, yet
include ../Makefile.tests_common

ifeq ($(BOARD),stm32f4discovery)
	include Makefile.$(BOARD)
endif

USEMODULE += net_if
USEMODULE += defaulttransceiver

include $(RIOTBASE)/Makefile.include
