APPLICATION = thread_msg_block_wo_queue
include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := nucleo32-f031

DISABLE_MODULE += auto_init

include $(RIOTBASE)/Makefile.include

test:
# `testrunner` calls `make term` recursively, results in duplicated `TERMFLAGS`.
# So clears `TERMFLAGS` before run.
	TERMFLAGS= tests/01-run.py
