include Makefile.modules

DIRS += $(foreach module, $(USEMODULE), $(MODULE_DIR_$(module)))

# Non handled wildcards
ifneq (,$(filter log_%,$(USEMODULE)))
  DIRS += log
endif
ifneq (,$(filter gnrc_%,$(USEMODULE)))
  DIRS += net/gnrc
endif

include $(RIOTBASE)/Makefile.base
