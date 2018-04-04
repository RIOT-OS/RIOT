include ../Makefile.tests_common

USEPKG += libfixmath
USEMODULE += libfixmath

TEST_ON_CI_WHITELIST += native

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
