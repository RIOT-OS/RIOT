_ALLMODULES = $(sort $(USEMODULE) $(USEPKG))

# Define MODULE_MODULE_NAME preprocessor macros for all modules.
ED = $(addprefix MODULE_,$(_ALLMODULES))
# EXTDEFINES will be put in CFLAGS_WITH_MACROS
EXTDEFINES = $(addprefix -D,$(call uppercase_and_underscore,$(ED)))

# filter "pseudomodules" from "real modules", but not "no_pseudomodules"
REALMODULES += $(filter-out $(PSEUDOMODULES), $(_ALLMODULES))
REALMODULES += $(filter $(NO_PSEUDOMODULES), $(_ALLMODULES))
BASELIBS += $(REALMODULES:%=$(BINDIR)/%.a)
