DEVELHELP ?= 0

include ../Makefile.sys_common

USEMODULE += ztimer_auto_adjust
USEMODULE += ztimer_overhead
USEMODULE += ztimer_usec

# microbit qemu timing is off
TEST_ON_CI_BLACKLIST += microbit

# The test is sensitive to background CPU load. On the CI workers a lot of
# compilation tasks are run in parallel, making this test randomly fail.
TEST_ON_CI_BLACKLIST += native32 native64

include $(RIOTBASE)/Makefile.include
