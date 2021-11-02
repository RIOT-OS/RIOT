ifeq (,$(filter rest_client_transport_%,$(USEMODULE)))
  $(error No transport protocol selected)
endif

DIRS += transport

ifneq (,$(filter rest_client_internal,$(USEMODULE)))
  DIRS += internal
endif

include $(RIOTBASE)/Makefile.base
