#
# Include this file if your Package needs to be checked out by git
#
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

GITFLAGS ?= -c user.email=buildsystem@riot -c user.name="RIOT buildsystem"
GITAMFLAGS ?= $(GIT_QUIET) --no-gpg-sign --ignore-whitespace --whitespace=nowarn

ifneq (,$(wildcard $(PKG_DIR)/patches))
$(PKG_BUILDDIR)/.git-patched: git-ensure-version $(PKG_DIR)/Makefile $(PKG_DIR)/patches/*.patch
	$(Q)git -C $(PKG_BUILDDIR) checkout $(GIT_QUIET) -f $(PKG_VERSION)
	$(Q)git $(GITFLAGS) -C $(PKG_BUILDDIR) am $(GITAMFLAGS) "$(PKG_DIR)"/patches/*.patch
	$(Q)touch $@
endif

git-ensure-version: $(PKG_BUILDDIR)/.git-downloaded
	@if [ $(shell git -C $(PKG_BUILDDIR) rev-parse HEAD) != $(PKG_VERSION) ] ; then \
		git -C $(PKG_BUILDDIR) clean $(GIT_QUIET) -xdff ; \
		git -C $(PKG_BUILDDIR) fetch $(GIT_QUIET) "$(PKG_URL)" "$(PKG_VERSION)" ; \
		git -C $(PKG_BUILDDIR) checkout $(GIT_QUIET) -f $(PKG_VERSION) ; \
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
		git -C $(PKG_BUILDDIR) clean -f ; \
		git -C $(PKG_BUILDDIR) checkout "$(PKG_VERSION)"; \
		make $(PKG_BUILDDIR)/.git-patched ; \
		touch $(PKG_BUILDDIR)/.git-downloaded ; \
	} > /dev/null 2>&1 || true

distclean::
	rm -rf "$(PKG_BUILDDIR)"

endif
