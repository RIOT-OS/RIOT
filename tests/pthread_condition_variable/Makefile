include ../Makefile.tests_common

BOARD_BLACKLIST := arduino-mega2560 waspmote-pro arduino-uno arduino-duemilanove \
                   jiminy-mega256rfr2

BOARD_INSUFFICIENT_MEMORY := nucleo32-f031

USEMODULE += posix
USEMODULE += pthread
USEMODULE += xtimer

CFLAGS += -DNATIVE_AUTO_EXIT

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
