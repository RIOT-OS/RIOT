MODULE = cord_epsim

SRC = cord_epsim.c

ifneq (,$(filter cord_epsim_standalone,$(USEMODULE)))
  SRC += cord_epsim_standalone.c
endif

include $(RIOTBASE)/Makefile.base
