#
# Include this file if your Package needs to be checked out by git
#
PKG_DIR?=$(CURDIR)
PKG_BUILDDIR?=$(PKGDIRBASE)/$(PKG_NAME)

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

GITFLAGS ?= -c user.email=buildsystem@riot -c user.name="RIOT buildsystem"
GITAMFLAGS ?= --no-gpg-sign --ignore-whitespace

ifneq (,$(wildcard $(PKG_DIR)/patches))
$(PKG_BUILDDIR)/.git-patched: git-ensure-version $(PKG_DIR)/Makefile $(PKG_DIR)/patches/*.patch
	git -C $(PKG_BUILDDIR) checkout -f $(PKG_VERSION)
	git $(GITFLAGS) -C $(PKG_BUILDDIR) am $(GITAMFLAGS) "$(PKG_DIR)"/patches/*.patch
	touch $@
endif

git-ensure-version: $(PKG_BUILDDIR)/.git-downloaded
	if [ $(shell git -C $(PKG_BUILDDIR) rev-parse HEAD) != $(PKG_VERSION) ] ; then \
		git -C $(PKG_BUILDDIR) clean -xdff ; \
		git -C $(PKG_BUILDDIR) fetch "$(PKG_URL)" "$(PKG_VERSION)" ; \
		git -C $(PKG_BUILDDIR) checkout -f $(PKG_VERSION) ; \
		touch $(PKG_BUILDDIR)/.git-downloaded ; \
	fi

$(PKG_BUILDDIR)/.git-downloaded:
	rm -Rf $(PKG_BUILDDIR)
	mkdir -p $(PKG_BUILDDIR)
	$(GITCACHE) clone "$(PKG_URL)" "$(PKG_VERSION)" "$(PKG_BUILDDIR)"
	touch $@

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
