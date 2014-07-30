APPLICATION = coap
include ../Makefile.tests_common

BOARD_BLACKLIST := arduino-due chronos mbed_lpc1768 msb-430 msb-430h qemu-i386 stm32f0discovery \
                   stm32f4discovery telosb wsn430-v1_3b wsn430-v1_4 udoo z1
BOARD_INSUFFICIENT_RAM := redbee-econotag
#MSP boards: no assert.h
#rest: no radio

USEMODULE += defaulttransceiver
USEPKG += libcoap

include $(RIOTBASE)/Makefile.include
