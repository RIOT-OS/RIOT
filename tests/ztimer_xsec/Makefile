include ../Makefile.tests_common

USEMODULE += ztimer
USEMODULE += ztimer_usec
USEMODULE += ztimer_msec
USEMODULE += ztimer_sec

# microbit qemu lacks rtt
TEST_ON_CI_BLACKLIST += microbit

include $(RIOTBASE)/Makefile.include
