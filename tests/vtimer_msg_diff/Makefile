APPLICATION = vtimer_msg_diff
include ../Makefile.tests_common

BOARD_BLACKLIST := arduino-mega2560
# arduino-mega2560: missing define for PRId64, avr-libc's printf function
# possibly not compatible at all

BOARD_INSUFFICIENT_RAM := mbed_lpc1768 stm32f0discovery pca10000 pca10005 samr21-xpro

USEMODULE += vtimer

include $(RIOTBASE)/Makefile.include
