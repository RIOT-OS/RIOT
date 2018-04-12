include ../Makefile.tests_common

USEMODULE += core_thread_flags
USEMODULE += xtimer

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
