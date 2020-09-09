include ../Makefile.tests_common

TEST_ZTIMER_CLOCK ?= ZTIMER_USEC

USEMODULE += ztimer
USEMODULE += ztimer_usec
ifeq ($(TEST_ZTIMER_CLOCK), ZTIMER_MSEC)
  USEMODULE += ztimer_msec
  USEMODULE += ztimer_periph_rtt
endif

CFLAGS += -DTEST_ZTIMER_CLOCK=$(TEST_ZTIMER_CLOCK)

include $(RIOTBASE)/Makefile.include
