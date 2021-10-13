# Perform a recursive dependency resolution: Include $(RIOTBASE)/Makefile.dep
# until no new modules, pkgs, or features are pull in order to catch all
# transient dependencies

# Locate used modules in $(EXTERNAL_MODULE_DIRS).
EXTERNAL_MODULE_PATHS := $(sort $(foreach dir,$(EXTERNAL_MODULE_DIRS),\
  $(foreach mod,$(USEMODULE),$(dir $(wildcard $(dir)/$(mod)/Makefile)))))

# Back up current state to detect changes
OLD_STATE := $(USEMODULE) $(USEPKG) $(FEATURES_USED)

# pull in dependencies of the currently used modules and pkgs
include $(RIOTBASE)/Makefile.dep

# check if required features are provided and update $(FEATURES_USED)
include $(RIOTMAKE)/features_check.inc.mk

# translate used features into used module, where needed
include $(RIOTMAKE)/features_modules.inc.mk

# sort and de-duplicate used modules, pkgs, and features for comparison
USEMODULE := $(sort $(USEMODULE))
USEPKG := $(sort $(USEPKG))
FEATURES_USED := $(sort $(FEATURES_USED))

NEW_STATE := $(USEMODULE) $(USEPKG) $(FEATURES_USED)

# If set of used modules, pkgs, and features has changed during last run, run
# again to recursively catch transitive dependencies
ifneq ($(OLD_STATE),$(NEW_STATE))
  include $(RIOTMAKE)/dependency_resolution.inc.mk
else
  # If module auto_init is not used, silently disable all of its submodules
  ifeq (,$(filter auto_init,$(USEMODULE)))
    DISABLE_MODULE += auto_init_%
  endif

  # add default modules again, as $(DEFAULT_MODULE) might have been extended
  # during dependency processing
  USEMODULE += $(filter-out $(DISABLE_MODULE),$(DEFAULT_MODULE))

  # Sort and de-duplicate used modules and default modules for readability
  USEMODULE := $(sort $(USEMODULE))
  DEFAULT_MODULE := $(sort $(DEFAULT_MODULE))

  # Warn about used deprecated modules
  include $(RIOTMAKE)/deprecated_modules.inc.mk
  DEPRECATED_MODULES_USED := $(sort $(filter $(DEPRECATED_MODULES),$(USEMODULE)))
  ifneq (,$(DEPRECATED_MODULES_USED))
    $(shell $(COLOR_ECHO) "$(COLOR_RED)Deprecated modules are in use:$(COLOR_RESET)"\
                          "$(DEPRECATED_MODULES_USED)" 1>&2)
  endif
endif
