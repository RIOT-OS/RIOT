# This is the root Kconfig
KCONFIG ?= $(RIOTBASE)/Kconfig

# Include tools targets
include $(RIOTMAKE)/tools/kconfiglib.inc.mk

# Generated dir will contain Kconfig generated configurations
GENERATED_DIR = $(BINDIR)/generated

# This file will contain all generated configuration from kconfig
export KCONFIG_GENERATED_AUTOCONF_HEADER_C = $(GENERATED_DIR)/autoconf.h

# Header for the generated header file
define KCONFIG_AUTOHEADER_HEADER
$(if $(filter MINGW% CYGWIN% MSYS%,$(OS)),/)/* RIOT Configuration File */

endef
export KCONFIG_AUTOHEADER_HEADER

# This file will contain the calculated dependencies formated in Kconfig
export KCONFIG_GENERATED_DEPENDENCIES = $(GENERATED_DIR)/Kconfig.dep

# This file will contain application default configurations
KCONFIG_APP_CONFIG = $(APPDIR)/app.config

# Default and user overwritten configurations
KCONFIG_USER_CONFIG = $(APPDIR)/user.config

# This file will contain merged configurations from MERGE_SOURCES and is the
# one that is used to generate the 'riotconf.h' header
KCONFIG_MERGED_CONFIG = $(GENERATED_DIR)/merged.config

# This is the output of the generated configuration. It always mirrors the
# content of KCONFIG_GENERATED_AUTOCONF_HEADER_C, and it is used to load
# configuration symbols to the build system.
KCONFIG_OUT_CONFIG = $(GENERATED_DIR)/out.config

# Include configuration symbols if available, only when not cleaning. This
# allows to check for Kconfig symbols in makefiles.
# Make tries to 'remake' all included files
# (see https://www.gnu.org/software/make/manual/html_node/Remaking-Makefiles.html).
# So if this file was included even when 'clean' is called, make would enter a
# loop, as it always is out-of-date.
# This has the side effect of requiring a Kconfig user to run 'clean' on a
# separate call (e.g. 'make clean && make all'), to get the symbols correctly.
ifneq ($(CLEAN),clean)
  -include $(KCONFIG_OUT_CONFIG)
endif

# Flag that indicates that the configuration has been edited
KCONFIG_EDITED_CONFIG = $(GENERATED_DIR)/.editedconfig

# Add configurations to merge, in ascendent priority (i.e. a file overrides the
# previous ones)
MERGE_SOURCES += $(wildcard $(KCONFIG_APP_CONFIG))
MERGE_SOURCES += $(wildcard $(KCONFIG_USER_CONFIG))

# Create directory to place generated files
$(GENERATED_DIR): $(CLEAN)
	$(Q)mkdir -p $@

# During migration this checks if Kconfig should run. It will run if any of
# the following is true:
# - A file with '.config' extension is present in the application directory
# - A 'Kconfig' file is present in the application directory
# - A previous configuration file is present (e.g. from a previous call to
#   menuconfig)
# - menuconfig is being called
#
# NOTE: This assumes that Kconfig will not generate any default configurations
# just from the Kconfig files outside the application folder (i.e. module
# configuration via Kconfig is disabled by default). Should this change, the
# check would not longer be valid, and Kconfig would have to run on every
# build.
SHOULD_RUN_KCONFIG ?= $(or $(wildcard $(APPDIR)/*.config), $(wildcard $(APPDIR)/Kconfig), $(wildcard $(KCONFIG_MERGED_CONFIG)), $(filter menuconfig, $(MAKECMDGOALS)))

ifneq (,$(SHOULD_RUN_KCONFIG))
# Add configuration header to build dependencies
BUILDDEPS += $(KCONFIG_GENERATED_AUTOCONF_HEADER_C)

# Include configuration header when building
CFLAGS += -include '$(KCONFIG_GENERATED_AUTOCONF_HEADER_C)'

USEMODULE_W_PREFIX = $(addprefix MODULE_,$(USEMODULE))
USEPKG_W_PREFIX = $(addprefix PKG_,$(USEPKG))

# Build a Kconfig file defining all used modules and packages. This is done by
# defining symbols like 'MODULE_<MODULE_NAME>' or PKG_<PACKAGE_NAME> which
# default to 'y'. Then, every module and package Kconfig menu will depend on
# that symbol being set to show its options.
$(KCONFIG_GENERATED_DEPENDENCIES): FORCE | $(GENERATED_DIR)
	$(Q)printf "%s " $(USEMODULE_W_PREFIX) $(USEPKG_W_PREFIX) \
	  | awk 'BEGIN {RS=" "}{ gsub("-", "_", $$0); \
	      printf "config %s\n\tbool\n\tdefault y\n", toupper($$0)}' \
	  | $(LAZYSPONGE) $(LAZYSPONGE_FLAGS) $@

.PHONY: menuconfig

# Opens the menuconfig interface for configuration of modules using the Kconfig
# system.
menuconfig: $(MENUCONFIG) $(KCONFIG_MERGED_CONFIG) $(KCONFIG_EDITED_CONFIG)
	$(Q)KCONFIG_CONFIG=$(KCONFIG_MERGED_CONFIG) $(MENUCONFIG) $(KCONFIG)

# Marks that the configuration file has been edited via some interface, such as
# menuconfig
$(KCONFIG_EDITED_CONFIG): FORCE
	$(Q)touch $(KCONFIG_EDITED_CONFIG)

# Generates a merged configuration file from the given sources. If the config
# file has been edited a '.editedconfig' file will be present.
# This is used to decide if the sources have to be merged or not.
$(KCONFIG_MERGED_CONFIG): $(MERGECONFIG) $(KCONFIG_GENERATED_DEPENDENCIES) FORCE
	$(Q)\
	if ! test -f $(KCONFIG_EDITED_CONFIG); then \
	  if ! test -z "$(strip $(MERGE_SOURCES))"; then \
	    $(MERGECONFIG) $(KCONFIG) $@ $(MERGE_SOURCES); \
	  else \
	    rm -f $@; \
	  fi \
	fi

# Build a header file with all the Kconfig configurations. genconfig will avoid
# any unnecessary rewrites of the header file if no configurations changed.
$(KCONFIG_OUT_CONFIG) $(KCONFIG_GENERATED_AUTOCONF_HEADER_C) &: $(KCONFIG_GENERATED_DEPENDENCIES) $(GENCONFIG) $(KCONFIG_MERGED_CONFIG) FORCE
	$(Q) \
	KCONFIG_CONFIG=$(KCONFIG_MERGED_CONFIG) $(GENCONFIG) \
	  --config-out=$(KCONFIG_OUT_CONFIG) \
	  --header-path $(KCONFIG_GENERATED_AUTOCONF_HEADER_C) $(KCONFIG)
endif
