APPLICATION = coap
include ../Makefile.tests_common

# msp430 and avr have problems with int width and libcoaps usage of :x notation in structs
BOARD_BLACKLIST := arduino-mega2560 chronos msb-430 msb-430h telosb wsn430-v1_3b wsn430-v1_4 z1
BOARD_INSUFFICIENT_MEMORY := chronos msb-430 msb-430h stm32f0discovery telosb \
                             wsn430-v1_3b wsn430-v1_4 z1

USEMODULE += gnrc_ipv6
USEPKG += libcoap

include $(RIOTBASE)/Makefile.include
