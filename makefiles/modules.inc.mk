_ALLMODULES = $(sort $(USEMODULE) $(USEPKG))

# Define MODULE_MODULE_NAME preprocessor macros for all modules.
ED = $(addprefix MODULE_,$(_ALLMODULES))
EXTDEFINES = $(addprefix -D,$(shell echo '$(ED)' | tr 'a-z-' 'A-Z_'))
CFLAGS += $(EXTDEFINES)

# filter "pseudomodules" from "real modules", but not "no_pseudomodules"
REALMODULES += $(filter-out $(PSEUDOMODULES), $(_ALLMODULES))
REALMODULES += $(filter $(NO_PSEUDOMODULES), $(_ALLMODULES))

# For 'shared libraries' every object must be included
SHARED_LIBS = $(filter-out $(LIBS), $(REALMODULES))
# For 'static libraries' only required object files are included
STATIC_LIBS = $(filter $(LIBS), $(REALMODULES))

# Warning: the shared/static libraries handling is not currently handled
# for all architectures. But correctly defining them allows future proof
# handling.

SHARED_LIBS_FILES = $(SHARED_LIBS:%=$(BINDIR)/%.a)
STATIC_LIBS_FILES = $(STATIC_LIBS:%=$(BINDIR)/%.a)
BASELIBS += $(SHARED_LIBS_FILES) $(STATIC_LIBS_FILES)
