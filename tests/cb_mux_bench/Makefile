include ../Makefile.tests_common

USEMODULE += cb_mux \
             xtimer

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include

test:
	./tests/01-run.py
