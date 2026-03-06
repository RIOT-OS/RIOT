SRC := registry.c init.c

SUBMODULES := 1

ifneq (,$(filter registry_namespace_%,$(USEMODULE)))
  DIRS += namespace
endif

include $(RIOTBASE)/Makefile.base
