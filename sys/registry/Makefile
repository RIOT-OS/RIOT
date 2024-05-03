SRC := registry.c init.c tree_traversal.c

SUBMODULES := 1

ifneq (,$(filter registry_namespace_%,$(USEMODULE)))
  DIRS += namespace
endif

ifneq (,$(filter registry_storage%,$(USEMODULE)))
  DIRS += storage
endif

include $(RIOTBASE)/Makefile.base
