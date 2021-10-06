#
# Include this file if your package needs to be checked out by git
#
# A package is up to date when its '.git-prepared' file is up to date.
# Any change to the package makefile will force updating the repository
#
# Packages should include this file just after defining the PKG_* variables
# This will ensure the variables are defined in the Makefile.

ifneq (,$(.DEFAULT_GOAL))
  $(error $(lastword $(MAKEFILE_LIST)) must be included at the beginning of the file after defining the PKG_* variables)
endif

ifeq (,$(PKG_NAME))
  $(error PKG_NAME not defined)
endif
ifeq (,$(PKG_URL))
  $(error PKG_URL not defined)
endif
ifeq (,$(PKG_VERSION))
  $(error PKG_VERSION not defined)
endif
ifeq (,$(PKG_LICENSE))
  $(error PKG_LICENSE not defined)
endif

PKG_DIR ?= $(CURDIR)
PKG_PATCH_DIR ?= $(PKG_DIR)/patches

PKG_BUILD_OUT_OF_SOURCE ?= 1
ifeq (1,$(PKG_BUILD_OUT_OF_SOURCE))
  PKG_SOURCE_DIR ?= $(PKGDIRBASE)/$(PKG_NAME)
  PKG_BUILD_DIR ?= $(BINDIR)/pkg-build/$(PKG_NAME)
else
  # in-source builds: packages are cloned within the application build
  # directory in a separate pkg tree
  PKG_SOURCE_DIR ?= $(BINDIR)/pkg/$(PKG_NAME)
  PKG_BUILD_DIR = $(PKG_SOURCE_DIR)
endif

PKG_SOURCE_LOCAL ?= $(PKG_SOURCE_LOCAL_$(shell echo $(PKG_NAME) | tr a-z- A-Z_))

# git-cache specific management: GIT_CACHE_DIR is exported only
# when cloning the repository.
GITCACHE ?= $(RIOTTOOLS)/git/git-cache
GIT_CACHE_DIR ?= $(HOME)/.gitcache
include $(RIOTBASE)/makefiles/utils/variables.mk
$(call target-export-variables,$(PKG_BUILDDIR)/.git,GIT_CACHE_DIR)

# allow overriding package source with local folder (useful during development)
ifneq (,$(PKG_SOURCE_LOCAL))
  include $(RIOTBASE)/pkg/local.mk
else

ifeq ($(QUIET),1)
  GIT_QUIET ?= --quiet
endif

GITFLAGS ?= -c user.email=buildsystem@riot -c user.name="RIOT buildsystem"
GITAMFLAGS ?= $(GIT_QUIET) --no-gpg-sign --ignore-whitespace --whitespace=nowarn

.PHONY: all prepare clean distclean FORCE

PKG_PATCHES = $(sort $(wildcard $(PKG_PATCH_DIR)/*.patch))

PKG_STATE_FILE = .pkg-state.git
PKG_STATE      = $(PKG_SOURCE_DIR)/$(PKG_STATE_FILE)

PKG_PREPARED   = $(PKG_STATE)-prepared
PKG_PATCHED    = $(PKG_STATE)-patched
PKG_DOWNLOADED = $(PKG_STATE)-downloaded

# Custom prepared target that can be defined in packages Makefile.
PKG_CUSTOM_PREPARED ?=

# Declare 'all' first to have it being the default target
all: prepare

# Add noop builtin to avoid "Nothing to be done for prepare" message
prepare: $(PKG_PREPARED)
	@:

# Allow packages to add a custom step to be `prepared`.
# It should be a dependency of `$(PKG_PREPARED)` and depend on `$(PKG_PATCHED)`
$(PKG_PREPARED): $(PKG_PATCHED)
	@touch $@

# Use explicit '--git-dir' and '--work-tree' to prevent issues when the
# directory is not a git repository for any reason (clean -xdff or others)
GIT_IN_PKG = git -C $(PKG_SOURCE_DIR) --git-dir=.git --work-tree=.

# When $(PKG_PATCHED).d is included $(PKG_PATCHED) prerequisites will include
# the old prerequisites forcing a rebuild on prerequisite removal, but we do
# not want to generate $(PKG_PATCHED).d with the old prerequisites
PKG_PATCHED_PREREQUISITES = $(PKG_PATCHES) $(PKG_DOWNLOADED) $(MAKEFILE_LIST)

# Generate dependency file. Force rebuilding on dependency deletion
# Warning: It will be evaluated before target execution, so use as first step
#   $1: output file name
gen_dependency_files = $(file >$1,$@: $2)$(foreach f,$2,$(file >>$1,$(f):))

# Patch the package
# * create dependencies files
# * clean, without removing the 'state' files
# * checkout the wanted base commit
# * apply patches if there are any. (If none, it does nothing)
$(PKG_PATCHED): $(PKG_PATCHED_PREREQUISITES)
	$(info [INFO] patch $(PKG_NAME))
	$(call gen_dependency_files,$@.d,$(PKG_PATCHED_PREREQUISITES))
	$(Q)$(GIT_IN_PKG) clean $(GIT_QUIET) -xdff '**' -e $(PKG_STATE:$(PKG_SOURCE_DIR)/%='%*')
	$(Q)$(GIT_IN_PKG) checkout $(GIT_QUIET) -f $(PKG_VERSION)
	$(Q)$(GIT_IN_PKG) $(GITFLAGS) am $(GITAMFLAGS) $(PKG_PATCHES) </dev/null
	@touch $@

$(PKG_DOWNLOADED): $(MAKEFILE_LIST) | $(PKG_SOURCE_DIR)/.git
	$(info [INFO] updating $(PKG_NAME) $(PKG_DOWNLOADED))
	$(Q)if ! $(GIT_IN_PKG) cat-file -e $(PKG_VERSION); then \
		printf "[INFO] fetching new $(PKG_NAME) version "$(PKG_VERSION)"\n"; \
		$(GIT_IN_PKG) fetch $(GIT_QUIET) "$(PKG_URL)" "$(PKG_VERSION)"; \
	fi
	echo $(PKG_VERSION) > $@

$(PKG_SOURCE_DIR)/.git: | $(PKG_CUSTOM_PREPARED)
	$(info [INFO] cloning $(PKG_NAME))
	$(Q)rm -Rf $(PKG_SOURCE_DIR)
	$(Q)mkdir -p $(PKG_SOURCE_DIR)
	$(Q)$(GITCACHE) clone $(PKG_URL) $(PKG_VERSION) $(PKG_SOURCE_DIR)

ifeq ($(PKG_SOURCE_DIR),$(PKG_BUILD_DIR))
# This is the case for packages that are built within their source directory
# e.g. micropython and openthread
clean::
	@-test -d $(PKG_SOURCE_DIR) && $(GIT_IN_PKG) clean -xdff '**' -e $(PKG_STATE:$(PKG_SOURCE_DIR)/%='%*')
else
clean::
	rm -rf $(PKG_BUILD_DIR)
endif

distclean:: clean
	rm -rf $(PKG_SOURCE_DIR)

# Dependencies to 'patches'
-include $(PKG_PATCHED).d

# Reset goal for package
.DEFAULT_GOAL =
endif
