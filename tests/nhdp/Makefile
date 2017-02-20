APPLICATION = nhdp
include ../Makefile.tests_common

BOARD_BLACKLIST := arduino-mega2560 chronos msb-430 msb-430h telosb \
                   wsn430-v1_3b wsn430-v1_4 z1 waspmote-pro arduino-uno \
                   arduino-duemilanove
BOARD_INSUFFICIENT_MEMORY := nucleo-f334 stm32f0discovery weio nucleo-f030 \
                             nucleo32-f042 nucleo32-f031 nucleo-l053

USEMODULE += gnrc_ipv6
USEMODULE += gnrc_sock_udp
USEMODULE += nhdp

include $(RIOTBASE)/Makefile.include
