MODULE = openwsn

SRC := $(filter-out sctimer_% icmpv6rpl.c,$(wildcard *.c))

ifneq (,$(filter openwsn_sctimer_rtt,$(USEMODULE)))
  SRC += sctimer_rtt.c
endif

ifneq (,$(filter openwsn_sctimer_ztimer,$(USEMODULE)))
  SRC += sctimer_ztimer.c
endif

ifeq (,$(filter openwsn_ipv6,$(USEMODULE)))
  SRC += icmpv6rpl.c
endif

include $(RIOTBASE)/Makefile.base
