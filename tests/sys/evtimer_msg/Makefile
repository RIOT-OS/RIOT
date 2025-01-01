include ../Makefile.sys_common

USEMODULE += evtimer
USEMODULE += ztimer_msec

# This test randomly fails on `native` so disable it from CI
TEST_ON_CI_BLACKLIST += native native32

# microbit qemu lacks rtt
TEST_ON_CI_BLACKLIST += microbit

include $(RIOTBASE)/Makefile.include
