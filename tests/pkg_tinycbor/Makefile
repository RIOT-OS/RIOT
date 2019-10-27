include ../Makefile.tests_common

# No 8 bit and 16 bit support yet
BOARD_BLACKLIST := arduino-duemilanove arduino-leonardo \
                   arduino-mega2560 arduino-nano \
                   arduino-uno atmega328p chronos microduino-corerf \
                   mega-xplained msb-430 msb-430h telosb waspmote-pro \
                   wsn430-v1_3b wsn430-v1_4 z1 \

USEMODULE += embunit
USEMODULE += fmt
USEPKG += tinycbor

USEMODULE += tinycbor_float

include $(RIOTBASE)/Makefile.include
