include ../Makefile.tests_common

# No 8 bit and 16 bit support
BOARD_BLACKLIST := arduino-duemilanove arduino-leonardo \
                   arduino-mega2560 arduino-nano \
                   arduino-uno chronos mega-xplained microduino-corerf \
                   msb-430 msb-430h telosb waspmote-pro wsn430-v1_3b \
                   wsn430-v1_4 z1

USEMODULE += embunit
USEMODULE += random
USEPKG += cifra

include $(RIOTBASE)/Makefile.include
