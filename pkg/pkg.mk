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

.PHONY: all prepare git-download clean distclean

PKG_PREPARED = $(PKG_BUILDDIR)/.git-prepared
PKG_PATCHES = $(sort $(wildcard $(PKG_DIR)/patches/*.patch))
PKG_PATCH_DEP_INC = $(PKG_BUILDDIR)/.git-patch-dep.inc

# Declare 'all' first to have it being the default target
all: $(PKG_PREPARED)
prepare: $(PKG_PREPARED)

# Compat with current packages
git-download: $(PKG_PREPARED)


# Create the makefile include file only on request
# It ensures rebuild of '.git-prepared' on patches/ deletion
.PHONY: _pkg_inc_file
_pkg_inc_file: | $(PKG_BUILDDIR)/.git
	@echo "$(PKG_BUILDDIR)/.git-prepared: $(PKG_PATCHES)" > $(PKG_PATCH_DEP_INC)
	@for patch in $(PKG_PATCHES); do echo "$(patch):" >> $(PKG_PATCH_DEP_INC); done

# Prepare the package
# * clean, without removing the pre-steps state files
# * checkout the wanted base commit
# * apply patches if there are any. (If none, it does nothing)
$(PKG_BUILDDIR)/.git-prepared: $(PKG_PATCHES) $(PKG_BUILDDIR)/.git-downloaded $(MAKEFILE_LIST) | _pkg_inc_file
	git -C $(PKG_BUILDDIR) clean -xdff '**' ':!.git-downloaded' ':!.git-patch-dep.inc'
	git -C $(PKG_BUILDDIR) checkout -f $(PKG_VERSION)
	git $(GITFLAGS) -C $(PKG_BUILDDIR) am $(GITAMFLAGS) $(PKG_PATCHES) </dev/null
	@touch $@

$(PKG_BUILDDIR)/.git-downloaded: $(MAKEFILE_LIST) | $(PKG_BUILDDIR)/.git
	git -C $(PKG_BUILDDIR) fetch $(PKG_URL) $(PKG_VERSION)
	echo $(PKG_VERSION) > $@

$(PKG_BUILDDIR)/.git: $(MAKEFILE_LIST)
	rm -Rf $(PKG_BUILDDIR)
	mkdir -p $(PKG_BUILDDIR)
	$(GITCACHE) clone $(PKG_URL) $(PKG_VERSION) $(PKG_BUILDDIR)

clean::
	@test -d $(PKG_BUILDDIR) && \
		git -C $(PKG_BUILDDIR) clean -xdff || \
		true

distclean::
	rm -rf $(PKG_BUILDDIR)

-include $(PKG_PATCH_DEP_INC)

# Reset goal for package
.DEFAULT_GOAL =
endif
