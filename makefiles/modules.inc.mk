USEMODULE := $(filter-out $(filter-out $(FEATURES_PROVIDED), $(FEATURES_OPTIONAL)), $(sort $(USEMODULE)))

ED = $(addprefix FEATURE_,$(sort $(filter $(FEATURES_PROVIDED), $(FEATURES_REQUIRED))))
ED += $(addprefix MODULE_,$(sort $(USEMODULE) $(USEPKG)))
EXTDEFINES = $(addprefix -D,$(shell echo '$(ED)' | tr 'a-z-' 'A-Z_'))
REALMODULES = $(filter-out $(PSEUDOMODULES), $(sort $(USEMODULE) $(USEPKG)))
export BASELIBS += $(REALMODULES:%=$(BINDIR)/%.a)

CFLAGS += $(EXTDEFINES)

export USEMODULE
