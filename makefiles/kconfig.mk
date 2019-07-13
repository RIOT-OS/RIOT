# This is the root Kconfig
KCONFIG ?= $(RIOTBASE)/Kconfig

# Include tools targets
include $(RIOTMAKE)/tools/kconfiglib.inc.mk

# Generated dir will contain Kconfig generated configurations
GENERATED_DIR = $(BINDIR)/generated

# This file will contain all generated configuration from kconfig
export KCONFIG_GENERATED_AUTOCONF_HEADER_C = $(GENERATED_DIR)/autoconf.h

# Add configuration header to build dependencies
BUILDDEPS += $(KCONFIG_GENERATED_AUTOCONF_HEADER_C)

# Include configuration header when building
CFLAGS += -include '$(KCONFIG_GENERATED_AUTOCONF_HEADER_C)'

# Header for the generated header file
define KCONFIG_AUTOHEADER_HEADER
/* RIOT Configuration File */

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

# Flag that indicates that the configuration has been edited
KCONFIG_EDITED_CONFIG = $(GENERATED_DIR)/.editedconfig

# Add configurations to merge, in ascendent priority (i.e. a file overrides the
# previous ones)
MERGE_SOURCES += $(wildcard $(KCONFIG_APP_CONFIG))
MERGE_SOURCES += $(wildcard $(KCONFIG_USER_CONFIG))

# Create directory to place generated files
$(GENERATED_DIR): $(CLEAN)
	$(Q)mkdir -p $@

# Build a Kconfig file defining all used modules. This is done by defining
# symbols like 'MODULE_<MODULE_NAME>' which default to 'y'. Then, every module
# Kconfig menu will depend on that symbol being set to show its options.
$(KCONFIG_GENERATED_DEPENDENCIES): FORCE | $(GENERATED_DIR)
	$(Q)printf "%s " $(USEMODULE) \
	  | awk 'BEGIN {RS=" "}{ gsub("-", "_", $$0); \
	      printf "config MODULE_%s\n\tbool\n\tdefault y\n", toupper($$0)}' \
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
$(KCONFIG_GENERATED_AUTOCONF_HEADER_C): $(KCONFIG_GENERATED_DEPENDENCIES) $(GENCONFIG) $(KCONFIG_MERGED_CONFIG) FORCE
	$(Q)KCONFIG_CONFIG=$(KCONFIG_MERGED_CONFIG) $(GENCONFIG) --header-path $@ $(KCONFIG)
