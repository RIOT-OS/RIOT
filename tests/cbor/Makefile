include ../Makefile.tests_common

BOARD_BLACKLIST := arduino-duemilanove arduino-mega2560 arduino-uno
BOARD_BLACKLIST += chronos
BOARD_BLACKLIST += mega-xplained
BOARD_BLACKLIST += mips-malta
BOARD_BLACKLIST += msb-430 msb-430h
BOARD_BLACKLIST += nucleo-f031k6
BOARD_BLACKLIST += pic32-clicker pic32-wifire
BOARD_BLACKLIST += telosb
BOARD_BLACKLIST += waspmote-pro
BOARD_BLACKLIST += wsn430-v1_3b wsn430-v1_4
BOARD_BLACKLIST += z1
BOARD_BLACKLIST += jiminy-mega256rfr2

USEMODULE += cbor
USEMODULE += cbor_ctime
USEMODULE += cbor_float
USEMODULE += cbor_semantic_tagging

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
