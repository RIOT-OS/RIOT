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
PKG_BUILDDIR ?= $(PKGDIRBASE)/$(PKG_NAME)
PKG_SOURCE_LOCAL ?= $(PKG_SOURCE_LOCAL_$(shell echo $(PKG_NAME) | tr a-z- A-Z_))

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

PKG_PATCHES = $(sort $(wildcard $(PKG_DIR)/patches/*.patch))

PKG_STATE_FILE = .pkg-state.git
PKG_STATE      = $(PKG_BUILDDIR)/$(PKG_STATE_FILE)

PKG_PREPARED   = $(PKG_STATE)-prepared
PKG_PATCHED    = $(PKG_STATE)-patched
PKG_DOWNLOADED = $(PKG_STATE)-downloaded

# Declare 'all' first to have it being the default target
all: $(PKG_PREPARED)
prepare: $(PKG_PREPARED)

# Allow packages to add a custom step to be `prepared`.
# It should be a dependency of `$(PKG_PREPARED)` and depend on `$(PKG_PATCHED)`
$(PKG_PREPARED): $(PKG_PATCHED)
	@touch $@

# Use explicit '--git-dir' and '--work-tree' to prevent issues when the
# directory is not a git repository for any reason (clean -xdff or others)
GIT_IN_PKG = git -C $(PKG_BUILDDIR) --git-dir=.git --work-tree=.

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
	$(Q)$(GIT_IN_PKG) clean $(GIT_QUIET) -xdff '**' -e $(PKG_STATE:$(PKG_BUILDDIR)/%='%*')
	$(Q)$(GIT_IN_PKG) checkout $(GIT_QUIET) -f $(PKG_VERSION)
	$(Q)$(GIT_IN_PKG) $(GITFLAGS) am $(GITAMFLAGS) $(PKG_PATCHES) </dev/null
	@touch $@

$(PKG_DOWNLOADED): $(MAKEFILE_LIST) | $(PKG_BUILDDIR)/.git
	$(info [INFO] updating $(PKG_NAME) $(PKG_DOWNLOADED))
	$(Q)$(GIT_IN_PKG) fetch $(GIT_QUIET) $(PKG_URL) $(PKG_VERSION)
	echo $(PKG_VERSION) > $@

$(PKG_BUILDDIR)/.git:
	$(info [INFO] cloning $(PKG_NAME))
	$(Q)rm -Rf $(PKG_BUILDDIR)
	$(Q)mkdir -p $(PKG_BUILDDIR)
	$(Q)$(GITCACHE) clone $(PKG_URL) $(PKG_VERSION) $(PKG_BUILDDIR)

clean::
	@-test -d $(PKG_BUILDDIR) && $(GIT_IN_PKG) clean $(GIT_QUIET) -xdff

distclean::
	rm -rf $(PKG_BUILDDIR)

# Dependencies to 'patches'
-include $(PKG_PATCHED).d

# Reset goal for package
.DEFAULT_GOAL =
endif
