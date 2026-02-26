ifneq (,$(filter registry_namespace_sys%,$(USEMODULE)))
  DIRS += sys
endif

ifneq (,$(filter registry_namespace_tests%,$(USEMODULE)))
  DIRS += tests
endif

include $(RIOTBASE)/Makefile.base
