#
# Include this file if your Package needs to be checked out by git
#
PKG_DIR?=$(CURDIR)
PKG_BUILDDIR?=$(PKGDIRBASE)/$(PKG_NAME)

# allow overriding package source with local folder (useful during development)
ifneq (,$(PKG_SOURCE_LOCAL))
  include $(RIOTBASE)/pkg/local.mk
else

.PHONY: prepare git-download clean

prepare: git-download

ifneq (,$(wildcard $(PKG_DIR)/patches))
git-download: $(PKG_BUILDDIR)/.git-patched
else
git-download: $(PKG_BUILDDIR)/.git-downloaded
endif

ifneq (,$(wildcard $(PKG_DIR)/patches))
$(PKG_BUILDDIR)/.git-patched: $(PKG_BUILDDIR)/.git-downloaded $(PKG_DIR)/Makefile $(PKG_DIR)/patches/*.patch
	$(Q)git -C $(PKG_BUILDDIR) checkout -f $(PKG_VERSION)
	$(Q)git -C $(PKG_BUILDDIR) am --ignore-whitespace "$(PKG_DIR)"/patches/*.patch
	$(Q)touch $@
endif

$(PKG_BUILDDIR)/.git-downloaded:
	$(Q)rm -Rf $(PKG_BUILDDIR)
	$(Q)mkdir -p $(PKG_BUILDDIR)
	$(Q)$(GITCACHE) clone "$(PKG_URL)" "$(PKG_VERSION)" "$(PKG_BUILDDIR)"
	$(Q)touch $@

clean::
	$(Q)test -d $(PKG_BUILDDIR) && { \
		rm $(PKG_BUILDDIR)/.git-patched ; \
		git -C $(PKG_BUILDDIR) clean -f ; \
		git -C $(PKG_BUILDDIR) checkout "$(PKG_VERSION)"; \
		make $(PKG_BUILDDIR)/.git-patched ; \
		touch $(PKG_BUILDDIR)/.git-downloaded ; \
	} > /dev/null 2>&1 || true

distclean::
	$(Q)rm -rf "$(PKG_BUILDDIR)"
