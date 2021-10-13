_ALLMODULES = $(sort $(USEMODULE) $(USEPKG))

# Define MODULE_MODULE_NAME preprocessor macros for all modules.
# Note: Some modules may be using the 'IS_USED' macro, defined in
# kernel_defines.h, please refer to its documentation if you change the way
# module macros are defined.
ED = $(addprefix MODULE_,$(_ALLMODULES))
# EXTDEFINES will be put in CFLAGS_WITH_MACROS
EXTDEFINES = $(addprefix -D,$(call uppercase_and_underscore,$(ED)))

# filter "pseudomodules" from "real modules", but not "no_pseudomodules"
REALMODULES += $(filter-out $(PSEUDOMODULES), $(_ALLMODULES))
REALMODULES += $(filter $(NO_PSEUDOMODULES), $(_ALLMODULES))
BASELIBS += $(REALMODULES:%=%.module)

# Add modules compiled with Makefile.base and extra archives
GENERATED_MODULES = $(notdir $(shell find $(BINDIR) -type d)) $(basename $(notdir $(ARCHIVES)))

NON_GENERATED_MODULES = $(filter-out $(GENERATED_MODULES),$(REALMODULES))
