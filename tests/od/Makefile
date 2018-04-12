include ../Makefile.tests_common

USEMODULE += od
# USEMODULE += od_string

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include

test:
ifeq (,$(filter od_string,$(USEMODULE)))
	tests/01-run.py
else
	tests/02-run.py
endif
