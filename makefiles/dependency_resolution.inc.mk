# Perform a recursive dependency resolution: Include $(RIOTBASE)/Makefile.dep
# until no new modules, pkgs, or features are pull in order to catch all
# transient dependencies

# Locate used modules in $(EXTERNAL_MODULE_DIRS).
EXTERNAL_MODULE_PATHS := $(sort $(foreach dir,$(EXTERNAL_MODULE_DIRS),\
  $(foreach mod,$(USEMODULE),$(dir $(wildcard $(dir)/$(mod)/Makefile)))))

# Locate used packages in $(RIOTPKG) or $(EXTERNAL_PKG_DIRS).
PKGDIRS := $(RIOTPKG) $(EXTERNAL_PKG_DIRS)
PKG_PATHS := $(sort $(foreach dir,$(PKGDIRS),\
  $(foreach pkg,$(USEPKG),$(dir $(wildcard $(dir)/$(pkg)/Makefile)))))

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
  # Include late allowing them to have been disabled during dependency resolution
  DEFAULT_MODULE += auto_init periph_init
  DEFAULT_MODULE += periph_init_leds
  DEFAULT_MODULE += periph_init_led0
  DEFAULT_MODULE += periph_init_led1
  DEFAULT_MODULE += periph_init_led2
  DEFAULT_MODULE += periph_init_led3
  DEFAULT_MODULE += periph_init_led4
  DEFAULT_MODULE += periph_init_led5
  DEFAULT_MODULE += periph_init_led6
  DEFAULT_MODULE += periph_init_led7

  USEMODULE += $(filter-out $(DISABLE_MODULE),auto_init periph_init)

  # If module auto_init is not used, silently disable all of its submodules
  ifeq (,$(filter auto_init,$(USEMODULE)))
    DISABLE_MODULE += auto_init_%
  endif

  # If module periph_init is not used, silently disable all of its submodules
  ifeq (,$(filter periph_init,$(USEMODULE)))
    DISABLE_MODULE += periph_init_%
  endif

  # Add default modules again, as $(DEFAULT_MODULE) might have been extended
  # during dependency resolution
  USEMODULE += $(filter-out $(DISABLE_MODULE),$(DEFAULT_MODULE))

  # Include eventual dependencies for default modules
  include $(RIOTMAKE)/default_modules.deps.mk

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

  # Warn about telnet
  ifneq (,$(filter auto_init_telnet,$(USEMODULE)))
    ifneq (1,$(I_UNDERSTAND_THAT_TELNET_IS_INSECURE))
      $(shell $(COLOR_ECHO) "$(COLOR_RED)Telnet will be started automatically, "\
                            "make sure you understand why this almost certainly "\
                            "is a REALLY BAD idea before proceeding!$(COLOR_RESET)" 1>&2)
      $(error I_UNDERSTAND_THAT_TELNET_IS_INSECURE must be set to 1 to proceed)
    else
      $(shell $(COLOR_ECHO) "$(COLOR_YELLOW)Telnet will be started automatically,"\
                            "don't run this on public networks!$(COLOR_RESET)" 1>&2)
    endif
  endif
endif
