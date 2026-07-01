SRC := runtime_config.c init.c

SUBMODULES := 1

ifneq (,$(filter runtime_config_namespace_%,$(USEMODULE)))
  DIRS += namespace
endif

include $(RIOTBASE)/Makefile.base
