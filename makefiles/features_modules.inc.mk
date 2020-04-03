# Add modules implementing used features
#
# This is done after the regular dependency resolution in Makefile.dep, as
# feature resolution depends on the used modules. As these modules however have
# no dependencies(except for periph_common), no dependency resolution is needed.

PERIPH_FEATURES := $(filter periph_%,$(FEATURES_USED))
# all periph features correspond to a periph submodule
# FEATURES_USED is defined in Makefile.features
USEMODULE += $(PERIPH_FEATURES)

# Add all USED periph_% init modules unless they are blacklisted
PERIPH_MODULES := $(filter-out periph_init% periph_common,$(filter periph_%,$(USEMODULE)))
PERIPH_INIT_MODULES := $(subst periph_,periph_init_,$(PERIPH_MODULES))
DEFAULT_MODULE += $(PERIPH_INIT_MODULES)

# select cpu_check_address pseudomodule if the corresponding feature is used
USEMODULE += $(filter cpu_check_address, $(FEATURES_USED))

# include periph_common if any periph_* driver is used
ifneq (,$(filter periph_%, $(USEMODULE)))
  USEMODULE += periph_common
endif
