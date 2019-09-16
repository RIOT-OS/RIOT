include ../Makefile.tests_common

# Issue with integer width
BOARD_BLACKLIST += arduino-duemilanove
BOARD_BLACKLIST += arduino-leonardo
BOARD_BLACKLIST += arduino-mega2560
BOARD_BLACKLIST += arduino-nano
BOARD_BLACKLIST += arduino-uno
BOARD_BLACKLIST += chronos
BOARD_BLACKLIST += jiminy-mega256rfr2
BOARD_BLACKLIST += mega-xplained
BOARD_BLACKLIST += msb-430
BOARD_BLACKLIST += msb-430h
BOARD_BLACKLIST += telosb
BOARD_BLACKLIST += waspmote-pro
BOARD_BLACKLIST += wsn430-v1_3b
BOARD_BLACKLIST += wsn430-v1_4
BOARD_BLACKLIST += z1

BOARD_INSUFFICIENT_MEMORY += nucleo-f031k6
BOARD_INSUFFICIENT_MEMORY += nucleo-f042k6
BOARD_INSUFFICIENT_MEMORY += nucleo-l031k6

USEMODULE += embunit

USEMODULE += crypto
USEMODULE += cipher_modes
CFLAGS += -DCRYPTO_THREEDES

include $(RIOTBASE)/Makefile.include
