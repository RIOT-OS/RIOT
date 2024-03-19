DEVELHELP ?= 0
include ../Makefile.sys_common

# The test depends on how fast and often the host schedules the RIOT native
# application. If the host is busy enough it won't schedule the process,
# delaying the application, thus delaying the time measurement. This happens
# often on ci with resulting offsets of +10ms
TEST_ON_CI_BLACKLIST += native native64

USEMODULE += fmt
USEMODULE += ztimer_usec ztimer_msec

# microbit qemu lacks rtt
TEST_ON_CI_BLACKLIST += microbit

include $(RIOTBASE)/Makefile.include
