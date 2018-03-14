ED = $(addprefix MODULE_,$(sort $(USEMODULE) $(USEPKG)))
EXTDEFINES = $(addprefix -D,$(shell echo '$(ED)' | tr 'a-z-' 'A-Z_'))

# filter "pseudomodules" from "real modules", but not "no_pseudomodules"
REALMODULES = $(filter-out $(LIBS), \
  $(filter-out $(PSEUDOMODULES), $(USEMODULE)) \
  $(filter $(NO_PSEUDOMODULES), $(USEMODULE)))
REALPKGS = $(filter-out $(LIBS), \
  $(filter-out $(PSEUDOMODULES), $(USEPKG)) \
  $(filter $(NO_PSEUDOMODULES), $(USEPKG)))
REALARCHIVES = $(filter $(LIBS),$(USEMODULE) $(USEPKG))
# filter out any duplicate object file names
BASELIBS += \
  $(shell printf %s '$(REALMODULES:%=$(BINDIR)/%.a) $(REALPKGS:%=$(BINDIR)/%.a)' | \
  tr ' ' '\n' | awk '!a[$$0]++')
BASELIBS += $(REALARCHIVES:%=$(BINDIR)/%.a)

CFLAGS += $(EXTDEFINES)

export USEMODULE
