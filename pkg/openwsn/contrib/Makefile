MODULE = openwsn

SRC := $(filter-out sctimer_% ,$(wildcard *.c))

ifneq (,$(filter openwsn_sctimer_rtt,$(USEMODULE)))
  SRC += sctimer_rtt.c
endif

ifneq (,$(filter openwsn_sctimer_ztimer,$(USEMODULE)))
  SRC += sctimer_ztimer.c
endif

include $(RIOTBASE)/Makefile.base
