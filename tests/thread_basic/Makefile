APPLICATION = thread_basic
include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := nucleo32-f031

DISABLE_MODULE += auto_init

include $(RIOTBASE)/Makefile.include

test:
	./tests/test_thread.py
