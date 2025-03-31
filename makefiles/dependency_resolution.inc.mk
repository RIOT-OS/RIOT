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

# include the global board features before the feature check as they depend
# on the USEMODULE, which is present after the `Makefile.dep` resolution
include $(RIOTBOARD)/Makefile.features

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
  include $(RIOTMAKE)/defaultmodules_no_recursive_deps.inc.mk

  # Add auto_init and periph_init so that if not used all periph_init% and
  # auto_init_% modules can be silently disabled
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
  include $(RIOTMAKE)/defaultmodules_deps.inc.mk

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

  # Detect provided / used / optional features that do not exist
  include $(RIOTMAKE)/features_existing.inc.mk
  FEATURES_NONEXISTING := $(sort $(filter-out $(FEATURES_EXISTING),$(FEATURES_PROVIDED)))
  ifneq (,$(FEATURES_NONEXISTING))
    $(error "The following non-existing features are provided by the board $(BOARD): $(FEATURES_NONEXISTING)")
  endif

  FEATURES_NONEXISTING := $(sort $(filter-out $(FEATURES_EXISTING),$(FEATURES_REQUIRED)))
  ifneq (,$(FEATURES_NONEXISTING))
    $(error "The following non-existing features are listed in FEATURES_REQUIRED: $(FEATURES_NONEXISTING)")
  endif

  FEATURES_NONEXISTING := $(sort $(filter-out $(FEATURES_EXISTING),$(subst |, ,$(FEATURES_REQUIRED_ANY))))
  ifneq (,$(FEATURES_NONEXISTING))
    $(error "The following non-existing features are listed in FEATURES_REQUIRED_ANY: $(FEATURES_NONEXISTING)")
  endif

  FEATURES_NONEXISTING := $(sort $(filter-out $(FEATURES_EXISTING),$(FEATURES_OPTIONAL)))
  ifneq (,$(FEATURES_NONEXISTING))
    $(error "The following non-existing features are listed in FEATURES_OPTIONAL: $(FEATURES_NONEXISTING)")
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

  # Warn about STDIO UDP
  ifneq (,$(filter stdio_udp,$(USEMODULE)))
    ifneq (1,$(I_UNDERSTAND_THAT_STDIO_UDP_IS_INSECURE))
      $(shell $(COLOR_ECHO) "$(COLOR_RED)stdio via UDP will be started automatically,"\
                            "make sure you understand why this almost certainly"\
                            "is a REALLY BAD idea before proceeding!$(COLOR_RESET)" 1>&2)
      $(error I_UNDERSTAND_THAT_STDIO_UDP_IS_INSECURE must be set to 1 to proceed)
    else
      $(shell $(COLOR_ECHO) "$(COLOR_YELLOW)stdio via UDP will be started automatically,"\
                            "don't run this on public networks!$(COLOR_RESET)" 1>&2)
    endif
  endif

  # Warn about PSA Crypto
  ifneq (,$(filter psa_crypto,$(USEMODULE)))
    $(shell $(COLOR_ECHO) "$(COLOR_YELLOW) You are going to use the PSA Crypto module,"\
                          "which is only partly implemented and not yet thouroughly tested.\n"\
                          "Please do not use this module in production, as it may introduce"\
                          "security issues!$(COLOR_RESET)" 1>&2)
  endif
endif
