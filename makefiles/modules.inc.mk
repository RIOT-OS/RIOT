ED = $(addprefix MODULE_,$(sort $(USEMODULE) $(USEPKG)))
EXTDEFINES = $(addprefix -D,$(shell echo '$(ED)' | tr 'a-z-' 'A-Z_'))

# filter "pseudomodules" from "real modules", but not "no_pseudomodules"
NO_PSEUDOMODULES := $(filter $(NO_PSEUDOMODULES), $(sort $(USEMODULE) $(USEPKG)))
REALMODULES = $(filter-out $(PSEUDOMODULES), $(sort $(USEMODULE) $(USEPKG))) $(NO_PSEUDOMODULES)
export BASELIBS += $(REALMODULES:%=$(BINDIR)/%.a)

CFLAGS += $(EXTDEFINES)

export USEMODULE
