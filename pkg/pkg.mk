#
# Include this file if your Package needs to be checked out by git
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

GITFLAGS ?= -c user.email=buildsystem@riot -c user.name="RIOT buildsystem"
GITAMFLAGS ?= --no-gpg-sign --ignore-whitespace

.PHONY: all prepare git-download clean distclean FORCE

PKG_PATCHES = $(sort $(wildcard $(PKG_DIR)/patches/*.patch))

PKG_STATE_FILE = .pkg-state.git
PKG_STATE = $(PKG_BUILDDIR)/$(PKG_STATE_FILE)

PKG_PREPARED   = $(PKG_STATE)-prepared
PKG_PATCHED    = $(PKG_STATE)-patched
PKG_DOWNLOADED = $(PKG_STATE)-downloaded

# Declare 'all' first to have it being the default target
all: $(PKG_PREPARED)
prepare: $(PKG_PREPARED)

# Compat with current packages
git-download: $(PKG_PREPARED)


# Allow packages to add a custom step to be `prepared`.
# It should be a dependency of `$(PKG_PREPARED)` and depend on `$(PKG_PATCHED)`
$(PKG_PREPARED): $(PKG_PATCHED)
	@touch $@


# Generate dependency file. Force rebuilding on dependency deletion
# Warning: It will be evaluated before target execution, so use as first step
#   $1: output file name
gen_dependency_files = $(file >$1,$@: $2)$(foreach f,$2,$(file >>$1,$(f):))

# Patch the package
# * create dependencies files
# * clean, without removing the 'state' files
# * checkout the wanted base commit
# * apply patches if there are any. (If none, it does nothing)
$(PKG_PATCHED): $(PKG_PATCHES) $(PKG_DOWNLOADED) $(MAKEFILE_LIST)
	$(call gen_dependency_files,$@.d,$^)
	git -C $(PKG_BUILDDIR) clean -xdff '**' $(PKG_STATE:$(PKG_BUILDDIR)/%=':!%*')
	git -C $(PKG_BUILDDIR) checkout -f $(PKG_VERSION)
	git $(GITFLAGS) -C $(PKG_BUILDDIR) am $(GITAMFLAGS) $(PKG_PATCHES) </dev/null
	@touch $@

$(PKG_DOWNLOADED): $(MAKEFILE_LIST) | $(PKG_BUILDDIR)/.git
	git -C $(PKG_BUILDDIR) fetch $(PKG_URL) $(PKG_VERSION)
	echo $(PKG_VERSION) > $@

$(PKG_BUILDDIR)/.git:
	rm -Rf $(PKG_BUILDDIR)
	mkdir -p $(PKG_BUILDDIR)
	$(GITCACHE) clone $(PKG_URL) $(PKG_VERSION) $(PKG_BUILDDIR)

clean::
	@-test -d $(PKG_BUILDDIR) && git -C $(PKG_BUILDDIR) clean -xdff

distclean::
	rm -rf $(PKG_BUILDDIR)


# Depencies to 'patches'
-include $(PKG_PATCHED).d

# Reset goal for package
.DEFAULT_GOAL =
endif
