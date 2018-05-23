include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := nucleo-f031k6 nucleo-f042k6

DISABLE_MODULE += auto_init

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
