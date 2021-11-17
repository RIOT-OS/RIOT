SRC := event.c

SUBMODULES := 1

ifneq (,$(filter event_timeout_ztimer,$(USEMODULE)))
  ifneq (,$(filter ztimer_usec,$(USEMODULE)))
    SUBMODULES_NO_SRC += timeout.c
  endif
endif

include $(RIOTBASE)/Makefile.base
