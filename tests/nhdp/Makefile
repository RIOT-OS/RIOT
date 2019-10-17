include ../Makefile.tests_common

BOARD_BLACKLIST := arduino-duemilanove arduino-leonardo \
                   arduino-mega2560 arduino-nano \
                   arduino-uno chronos mega-xplained microduino-corerf \
                   msb-430 msb-430h telosb waspmote-pro \
                   wsn430-v1_3b wsn430-v1_4 z1

USEMODULE += gnrc_ipv6
USEMODULE += gnrc_sock_udp
USEMODULE += nhdp

include $(RIOTBASE)/Makefile.include
