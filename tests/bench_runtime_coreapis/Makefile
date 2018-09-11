include ../Makefile.tests_common

# we use thread flags in this benchmark by default, disable on demand
USEMODULE += core_thread_flags
USEMODULE += benchmark

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
