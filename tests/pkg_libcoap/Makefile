APPLICATION = pkg_libcoap
include ../Makefile.tests_common

# msp430 and avr have problems with int width and libcoaps usage of :x notation in structs
BOARD_BLACKLIST := arduino-mega2560 chronos msb-430 msb-430h telosb wsn430-v1_3b \
                   wsn430-v1_4 z1 waspmote-pro arduino-uno arduino-duemilanove
BOARD_INSUFFICIENT_MEMORY := chronos msb-430 msb-430h nucleo-f334 nucleo-f030 \
                             stm32f0discovery telosb weio wsn430-v1_3b wsn430-v1_4 z1 \
                             nucleo-f070 nucleo32-f042 nucleo32-f031 nucleo-l053

USEMODULE += gnrc_ipv6
USEMODULE += gnrc_sock_udp
USEPKG += libcoap

include $(RIOTBASE)/Makefile.include
