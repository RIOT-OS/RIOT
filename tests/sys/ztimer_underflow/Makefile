include ../Makefile.sys_common

TEST_ZTIMER_CLOCK ?= ZTIMER_USEC

USEMODULE += ztimer
USEMODULE += ztimer_usec
ifeq ($(TEST_ZTIMER_CLOCK), ZTIMER_MSEC)
  USEMODULE += ztimer_msec
endif
ifeq ($(TEST_ZTIMER_CLOCK), ZTIMER_SEC)
  USEMODULE += ztimer_sec
endif

CFLAGS += -DTEST_ZTIMER_CLOCK=$(TEST_ZTIMER_CLOCK)

# microbit qemu failing currently
TEST_ON_CI_BLACKLIST += microbit

# This test randomly fails on `native` so disable it from CI
TEST_ON_CI_BLACKLIST += native32 native64

include $(RIOTBASE)/Makefile.include
