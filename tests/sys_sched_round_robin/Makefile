include ../Makefile.tests_common

# Set to 1 to disable the round-robin scheduling module
NORR ?= 0

ifneq (1,$(NORR))
  USEMODULE += sched_round_robin
endif

include $(RIOTBASE)/Makefile.include

# Increase time between round-robin calls so prints are not interrupted for
# slow CPU's
ifneq (,$(filter ztimer_usec,$(USEMODULE)))
  CFLAGS += -DSCHED_RR_TIMEOUT=100*1000
else
ifneq (,$(filter ztimer_msec,$(USEMODULE)))
  CFLAGS += -DSCHED_RR_TIMEOUT=100
endif
endif
