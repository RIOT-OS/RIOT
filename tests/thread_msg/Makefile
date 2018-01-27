include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := nucleo32-f031 nucleo32-f042

DISABLE_MODULE += auto_init

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
