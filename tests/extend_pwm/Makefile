include ../Makefile.tests_common

USEMODULE += extend_pwm

TEST_ON_CI_WHITELIST += all

include ./Makefile.include
include $(RIOTBASE)/Makefile.include

test:
	./tests/01-run.py
