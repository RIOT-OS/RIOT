APPLICATION = event
include ../Makefile.tests_common

FORCE_ASSERTS = 1
USEMODULE += event_callback
USEMODULE += event_timeout

test:
	tests/01-run.py

include $(RIOTBASE)/Makefile.include
