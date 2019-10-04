#
# Include this file if your Package needs to be checked out by git
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

PKG_DIR?=$(CURDIR)
PKG_BUILDDIR?=$(PKGDIRBASE)/$(PKG_NAME)
PKG_SOURCE_LOCAL ?= $(PKG_SOURCE_LOCAL_$(shell echo $(PKG_NAME) | tr a-z- A-Z_))

# allow overriding package source with local folder (useful during development)
ifneq (,$(PKG_SOURCE_LOCAL))
  include $(RIOTBASE)/pkg/local.mk
else

.PHONY: prepare git-download clean git-ensure-version

prepare: git-download

ifneq (,$(wildcard $(PKG_DIR)/patches))
git-download: $(PKG_BUILDDIR)/.git-patched
else
git-download: git-ensure-version
endif

ifeq ($(QUIET),1)
  GIT_QUIET ?= --quiet
endif

# Use explicit '--git-dir' and '--work-tree' to prevent issues when the
# directory is not a git repository for any reason (clean -xdff or others)
GIT_IN_PKG = git -C $(PKG_BUILDDIR) --git-dir=.git --work-tree=.

GITFLAGS ?= -c user.email=buildsystem@riot -c user.name="RIOT buildsystem"
GITAMFLAGS ?= $(GIT_QUIET) --no-gpg-sign --ignore-whitespace --whitespace=nowarn

ifneq (,$(wildcard $(PKG_DIR)/patches))
$(PKG_BUILDDIR)/.git-patched: git-ensure-version $(PKG_DIR)/Makefile $(PKG_DIR)/patches/*.patch
	$(Q)$(GIT_IN_PKG) checkout $(GIT_QUIET) -f $(PKG_VERSION)
	$(Q)$(GIT_IN_PKG) $(GITFLAGS) am $(GITAMFLAGS) "$(PKG_DIR)"/patches/*.patch
	$(Q)touch $@
endif

git-ensure-version: $(PKG_BUILDDIR)/.git-downloaded
	@if [ $(shell $(GIT_IN_PKG) rev-parse HEAD) != $(PKG_VERSION) ] ; then \
		$(GIT_IN_PKG) clean $(GIT_QUIET) -xdff ; \
		$(GIT_IN_PKG) fetch $(GIT_QUIET) "$(PKG_URL)" "$(PKG_VERSION)" ; \
		$(GIT_IN_PKG) checkout $(GIT_QUIET) -f $(PKG_VERSION) ; \
		touch $(PKG_BUILDDIR)/.git-downloaded ; \
	fi

$(PKG_BUILDDIR)/.git-downloaded:
	$(Q)rm -Rf $(PKG_BUILDDIR)
	$(Q)mkdir -p $(PKG_BUILDDIR)
	$(Q)$(GITCACHE) clone "$(PKG_URL)" "$(PKG_VERSION)" "$(PKG_BUILDDIR)"
	$(Q)touch $@

clean::
	@test -d $(PKG_BUILDDIR) && { \
		rm $(PKG_BUILDDIR)/.git-patched ; \
		$(GIT_IN_PKG) clean -f ; \
		$(GIT_IN_PKG) checkout "$(PKG_VERSION)"; \
		make $(PKG_BUILDDIR)/.git-patched ; \
		touch $(PKG_BUILDDIR)/.git-downloaded ; \
	} > /dev/null 2>&1 || true

distclean::
	rm -rf "$(PKG_BUILDDIR)"

# Reset goal for package
.DEFAULT_GOAL =
endif
