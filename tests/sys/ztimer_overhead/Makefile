DEVELHELP ?= 0

include ../Makefile.sys_common

USEMODULE += ztimer_auto_adjust
USEMODULE += ztimer_overhead
USEMODULE += ztimer_usec

# microbit qemu timing is off
TEST_ON_CI_BLACKLIST += microbit

include $(RIOTBASE)/Makefile.include
