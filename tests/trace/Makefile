# name of your application
APPLICATION = trace
include ../Makefile.tests_common

USEMODULE += trace

BOARD_WHITELIST := native

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include

test:
# `testrunner` calls `make term` recursively, results in duplicated `TERMFLAGS`.
# So clears `TERMFLAGS` before run.
	TERMFLAGS= tests/01-run.py
