include ../Makefile.tests_common

# msp430 and avr have problems with int width and libcoaps usage of :x notation in structs
BOARD_BLACKLIST := arduino-duemilanove arduino-leonardo \
                   arduino-mega2560 arduino-nano \
                   arduino-uno chronos mega-xplained msb-430 \
                   msb-430h telosb waspmote-pro wsn430-v1_3b wsn430-v1_4 z1
BOARD_INSUFFICIENT_MEMORY := chronos i-nucleo-lrwan1 msb-430 msb-430h \
                             nucleo-f031k6 nucleo-f042k6 \
                             nucleo-l031k6 nucleo-f030r8 nucleo-f070rb \
                             nucleo-f303k8 nucleo-f334r8 nucleo-l053r8 \
                             stm32f0discovery stm32l0538-disco telosb wsn430-v1_3b \
                             wsn430-v1_4 z1

USEMODULE += gnrc_ipv6
USEMODULE += gnrc_sock_udp
USEPKG += libcoap

include $(RIOTBASE)/Makefile.include
