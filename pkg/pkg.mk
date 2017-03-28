#
# Include this file if your Package needs to be checked out by git
#
PKG_DIR?=$(CURDIR)
PKG_BUILDDIR?=$(PKGDIRBASE)/$(PKG_NAME)

.PHONY: git-download clean

ifneq (,$(wildcard $(PKG_DIR)/patches))
git-download: $(PKG_BUILDDIR)/.git-patched
else
git-download: $(PKG_BUILDDIR)/.git-downloaded
endif

ifneq (,$(wildcard $(PKG_DIR)/patches))
$(PKG_BUILDDIR)/.git-patched: $(PKG_BUILDDIR)/.git-downloaded $(PKG_DIR)/Makefile $(PKG_DIR)/patches/*.patch
	git -C $(PKG_BUILDDIR) checkout -f $(PKG_VERSION)
	git -C $(PKG_BUILDDIR) am --ignore-whitespace "$(PKG_DIR)"/patches/*.patch
	touch $@
endif

$(PKG_BUILDDIR)/.git-downloaded:
	rm -Rf $(PKG_BUILDDIR)
	mkdir -p $(PKG_BUILDDIR)
	$(GITCACHE) clone "$(PKG_URL)" "$(PKG_VERSION)" "$(PKG_BUILDDIR)"
	$(GIT_APPLY_PATCHES)
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
