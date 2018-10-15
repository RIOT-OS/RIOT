MODULE = cord_ep

SRC = cord_ep.c

ifneq (,$(filter cord_ep_standalone,$(USEMODULE)))
  SRC += cord_ep_standalone.c
endif

include $(RIOTBASE)/Makefile.base
