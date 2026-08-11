_ALLMODULES = $(sort $(USEMODULE) $(USEPKG))

# Define `MODULE_<MODULE_NAME>` preprocessor macros for all modules listed in
# `$(MODULE_DEFS_NEEDED)`. In case the module is used, they will be defined as
# 1, otherwise as 0.
#
# Modules not listed in `$(MODULE_DEFS_NEEDED)` will not be exposed to not
# add more entropy to `CFLAGS` as needed. This increases the chances that the
# ccache cache can be used and CI time will be reduced as a result.
ifneq (,$(MODULE_DEFS_NEEDED))
  EXTDEFINES := $(addsuffix =1, $(filter $(MODULE_DEFS_NEEDED), $(USEMODULE)))
  EXTDEFINES += $(addsuffix =0, $(filter-out $(USEMODULE), $(MODULE_DEFS_NEEDED)))
  EXTDEFINES := $(call uppercase_and_underscore, $(EXTDEFINES))
  EXTDEFINES := $(addprefix -DMODULE_,$(EXTDEFINES))
  EXTDEFINES := $(sort $(EXTDEFINES))
  MODULE_DEFS_NEEDED :=
endif

# filter "pseudomodules" from "real modules", but not "no_pseudomodules"
REALMODULES += $(filter-out $(PSEUDOMODULES), $(_ALLMODULES))
REALMODULES += $(filter $(NO_PSEUDOMODULES), $(_ALLMODULES))
BASELIBS += $(REALMODULES:%=%.module)

# Add modules compiled with Makefile.base and extra archives
GENERATED_MODULES = $(notdir $(shell find $(BINDIR) -type d)) $(basename $(notdir $(ARCHIVES)))

NON_GENERATED_MODULES = $(filter-out $(GENERATED_MODULES),$(REALMODULES))
