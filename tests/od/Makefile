# name of your application
APPLICATION = od
include ../Makefile.tests_common

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

USEMODULE += od

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

include $(RIOTBASE)/Makefile.include

test:
# `testrunner` calls `make term` recursively, results in duplicated `TERMFLAGS`.
# So clears `TERMFLAGS` before run.
	TERMFLAGS= tests/01-run.py
