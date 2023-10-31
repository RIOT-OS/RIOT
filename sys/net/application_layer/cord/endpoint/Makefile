MODULE = cord_endpoint_simple

SRC = cord_endpoint.c

ifneq (,$(filter cord_endpoint_singleton,$(USEMODULE)))
  SRC += cord_endpoint_singleton.c
endif

include $(RIOTBASE)/Makefile.base
