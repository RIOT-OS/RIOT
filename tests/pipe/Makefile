include ../Makefile.tests_common

#malloc.h not found
BOARD_BLACKLIST := jiminy-mega256rfr2 mega-xplained
BOARD_INSUFFICIENT_MEMORY := nucleo32-f031

USEMODULE += pipe

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
