include ../Makefile.tests_common

BOARD_BLACKLIST := arduino-mega2560 waspmote-pro arduino-uno arduino-duemilanove \
                   jiminy-mega256rfr2
# arduino mega2560 uno duemilanove : unknown type name: clockid_t
# jiminy-mega256rfr2: unknown type name: clockid_t

USEMODULE += pthread
USEMODULE += xtimer
USEMODULE += random

BOARD_INSUFFICIENT_MEMORY += chronos msb-430 msb-430h nucleo32-f031 \
                             nucleo32-f042 nucleo32-l031 nucleo-f030 \
                             nucleo-f334 nucleo-l053 stm32f0discovery

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
