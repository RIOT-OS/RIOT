include ../Makefile.tests_common

USEMODULE += evtimer
USEMODULE += ztimer_usec

# This test randomly fails on `native` so disable it from CI
TEST_ON_CI_BLACKLIST += native

include $(RIOTBASE)/Makefile.include
