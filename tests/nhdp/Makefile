APPLICATION = nhdp
include ../Makefile.tests_common

BOARD_BLACKLIST := arduino-mega2560 chronos msb-430 msb-430h telosb wsn430-v1_3b wsn430-v1_4 z1
BOARD_INSUFFICIENT_MEMORY := nucleo-f334 stm32f0discovery weio

USEMODULE += gnrc_ipv6
USEMODULE += gnrc_conn_udp
USEMODULE += nhdp

include $(RIOTBASE)/Makefile.include
