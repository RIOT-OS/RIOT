# Debug targets to evaluate dependency migrations
#
# The goal is to get the value of variables used for dependency resolution
# and the impact of refactoring

# Files output can be generated through the normal resolution with executing
# 'dependency-debug' for each board
#
#     BOARD=board_name make dependency-debug
#
# And the 'quick' version that is used by murdock with `info-boards-supported`.
# This one currently uses only a subset of the files/values required for
# dependency resolution.
#
#     DEPENDENCY_DEBUG=1 make info-boards-supported
#
#
# To compare in an aggregated file, you can run in an application directory:
#
#    for board in $(make info-boards); do DEPENDENCY_DEBUG_OUTPUT_DIR=bin/info BOARD=${board} make dependency-debug; done; cat bin/info/* > bin/deps_info
#    DEPENDENCY_DEBUG=1 DEPENDENCY_DEBUG_OUTPUT_DIR=bin/info-global make info-boards-supported; cat bin/info-global/* > bin/deps_info-boards-supported
#    # And compare both files
#    diff -u bin/deps_info bin/deps_info-boards-supported
#
# And when comparing two revisions, include the revision in the file names

.PHONY: dependency-debug dependency-debug-features-provided-kconfig
# Only generate the dependencies when the board is not disabled
# This will allow comparing with the output of `info-boards-supported` more easily
dependency-debug:
ifneq (,$(filter-out $(BOARD_BLACKLIST),$(filter $(if $(BOARD_WHITELIST),$(BOARD_WHITELIST), %),$(BOARD))))
	$(call file_save_dependencies_variables,dependencies_info)
	@:
else
	@echo Skipping $(BOARD) is not whitelisted or blacklisted
endif

# Generate a list of features for an specific board from Kconfig symbols. Make
# sure that SHOULD_RUN_KCONFIG variable is set
dependency-debug-features-provided-kconfig: $(KCONFIG_OUT_CONFIG)
	@for i in $(sort $(FEATURES_PROVIDED_KCONFIG)); do echo $$i; done

# Variable to debug modeled features in Kconfig files. As a convention features
# are represented by symbols with the `HAS_` prefix (so
# FEATURES_PROVIDED += periph_gpio would be HAS_PERIPH_GPIO). This filters
# all the generated symbols beginning with that prefix.
FEATURES_PROVIDED_KCONFIG = $(call lowercase,$(patsubst CONFIG_HAS_%,%,$(filter CONFIG_HAS_%, $(.VARIABLES))))

DEPENDENCY_DEBUG_OUTPUT_DIR ?= $(CURDIR)

# Save variables that are used for parsing dependencies
_DEPS_DEBUG_VARS += TOOLCHAIN
_DEPS_DEBUG_VARS += BOARD CPU CPU_MODEL CPU_FAM CPU_CORE CPU_ARCH
_DEPS_DEBUG_VARS += FEATURES_PROVIDED _FEATURES_PROVIDED_SORTED
_DEPS_DEBUG_VARS += FEATURES_REQUIRED _FEATURES_REQUIRED_SORTED
_DEPS_DEBUG_VARS += FEATURES_REQUIRED_ANY _FEATURES_REQUIRED_ANY_SORTED
_DEPS_DEBUG_VARS += FEATURES_OPTIONAL FEATURES_USED FEATURES_MISSING
_DEPS_DEBUG_VARS += FEATURES_CONFLICT FEATURES_CONFLICTING
_DEPS_DEBUG_VARS += USEMODULE DEFAULT_MODULE DISABLE_MODULE
DEPS_DEBUG_VARS ?= $(_DEPS_DEBUG_VARS)

_FEATURES_PROVIDED_SORTED = $(sort $(FEATURES_PROVIDED))
_FEATURES_REQUIRED_SORTED = $(sort $(FEATURES_REQUIRED))
_FEATURES_REQUIRED_ANY_SORTED = $(sort $(FEATURES_REQUIRED_ANY))

file_save_dependencies_variables = $(call file_save_variable,$(DEPENDENCY_DEBUG_OUTPUT_DIR)/$1_$(BOARD),$(DEPS_DEBUG_VARS))
# Remove file before to be sure appending is started with an empty file
file_save_variable = $(shell mkdir -p $(dir $1); rm -f $1)$(foreach v,$2,$(file >>$1,$(call _print_var,$v)))

# print variables sorted, this can eliminate false positives but will not allow
# to tell in what order the variables where updated.
DEPENDENCY_DEBUG_SORT_VARS ?= 0
# Remove spaces in case of empty value
# Remove spaces around value as it happens
ifneq (1,$(DEPENDENCY_DEBUG_SORT_VARS))
  _print_var = $(strip $1 = $(strip $($1)))
else
  _print_var = $(sort $(strip $1 = $(strip $($1))))
endif
