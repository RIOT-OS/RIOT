_ALLMODULES = $(sort $(USEMODULE) $(USEPKG))

ED = $(addprefix MODULE_,$(_ALLMODULES))
EXTDEFINES = $(addprefix -D,$(shell echo '$(ED)' | tr 'a-z-' 'A-Z_'))

# filter "pseudomodules" from "real modules", but not "no_pseudomodules"
NO_PSEUDOMODULES := $(filter $(NO_PSEUDOMODULES), $(_ALLMODULES))
REALMODULES = $(filter-out $(PSEUDOMODULES), $(_ALLMODULES)) $(NO_PSEUDOMODULES)
BASELIBS += $(REALMODULES:%=$(BINDIR)/%.a)

CFLAGS += $(EXTDEFINES)
