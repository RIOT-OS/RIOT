_ALLMODULES = $(sort $(USEMODULE) $(USEPKG))

# Define MODULE_MODULE_NAME preprocessor macros for all modules.
ED = $(addprefix MODULE_,$(_ALLMODULES))
EXTDEFINES = $(addprefix -D,$(shell echo '$(ED)' | tr 'a-z-' 'A-Z_'))
CFLAGS += $(EXTDEFINES)

# filter "pseudomodules" from "real modules", but not "no_pseudomodules"
REALMODULES += $(filter-out $(PSEUDOMODULES), $(_ALLMODULES))
REALMODULES += $(filter $(NO_PSEUDOMODULES), $(_ALLMODULES))


# Modules that can be linked by including all objects
OBJECTS_REALMODULES = $(filter-out $(LIBS), $(REALMODULES))
# Modules that must be linked using the default 'archive' linking behaviour
ARCHIVE_REALMODULES = $(filter $(LIBS), $(REALMODULES))

OBJECTS_LIBS += $(OBJECTS_REALMODULES:%=$(BINDIR)/%.a)
ARCHIVE_LIBS += $(ARCHIVE_REALMODULES:%=$(BINDIR)/%.a)

# Warning: the object/archive compilation handling is not currently handled
# for all architectures.

BASELIBS += $(OBJECTS_LIBS) $(ARCHIVE_LIBS)
