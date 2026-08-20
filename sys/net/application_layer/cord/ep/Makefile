MODULE = cord_ep

MODULE_DEFS_NEEDED += \
  cord_ep_standalone \
  #

SRC = cord_ep.c

ifneq (,$(filter cord_ep_standalone,$(USEMODULE)))
  SRC += cord_ep_standalone.c
endif

include $(RIOTBASE)/Makefile.base
