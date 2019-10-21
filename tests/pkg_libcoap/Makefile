include ../Makefile.tests_common

# msp430 and avr have problems with int width and libcoaps usage of :x notation in structs
BOARD_BLACKLIST := arduino-duemilanove arduino-leonardo \
                   arduino-mega2560 arduino-nano arduino-uno atmega328p \
                   chronos mega-xplained microduino-corerf msb-430 \
                   msb-430h telosb waspmote-pro wsn430-v1_3b wsn430-v1_4 z1

USEMODULE += gnrc_ipv6
USEMODULE += gnrc_sock_udp
USEPKG += libcoap

include $(RIOTBASE)/Makefile.include
