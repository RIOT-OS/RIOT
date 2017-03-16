APPLICATION = thread_flood
include ../Makefile.tests_common

DISABLE_MODULE += auto_init

include $(RIOTBASE)/Makefile.include

test:
# `testrunner` calls `make term` recursively, results in duplicated `TERMFLAGS`.
# So clears `TERMFLAGS` before run.
	TERMFLAGS= tests/test_thread.py
