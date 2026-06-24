MODULE := configuration

SUBMODULES := 1

SRC := configuration.c default_handlers.c

ifneq (,$(filter configuration_backend_%,$(USEMODULE)))
  DIRS += backend
endif

include $(RIOTBASE)/Makefile.base
