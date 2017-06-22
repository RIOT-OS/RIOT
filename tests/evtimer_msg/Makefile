APPLICATION = evtimer_msg
include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := nucleo32-f031 nucleo32-f042

USEMODULE += evtimer

include $(RIOTBASE)/Makefile.include

test:
# `testrunner` calls `make term` recursively, results in duplicated `TERMFLAGS`.
# So clears `TERMFLAGS` before run.
	TERMFLAGS= tests/01-run.py
