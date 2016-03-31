#
# Include this file if your Package needs to be checked out by git
#
PKG_DIR?=$(CURDIR)
PKG_BUILDDIR?=$(BINDIRBASE)/pkg/$(BOARD)/$(PKG_NAME)

.PHONY: git-download

git-download: $(PKG_BUILDDIR)/.git-downloaded

GIT_APPLY_PATCHES:=if test -d "$(PKG_DIR)"/patches; then \
	git -C "$(PKG_BUILDDIR)" am --ignore-whitespace "$(PKG_DIR)"/patches/*.patch; \
	fi

$(PKG_BUILDDIR)/.git-downloaded:
	rm -Rf $(PKG_BUILDDIR)
	mkdir -p $(PKG_BUILDDIR)
	$(GITCACHE) clone "$(PKG_URL)" "$(PKG_VERSION)" "$(PKG_BUILDDIR)"
	$(GIT_APPLY_PATCHES)
	touch $@

clean::
	@test -d $(PKG_BUILDDIR) && { \
		git -C $(PKG_BUILDDIR) clean -f ; \
		git -C $(PKG_BUILDDIR) checkout "$(PKG_VERSION)"; \
		$(GIT_APPLY_PATCHES) ; \
		touch $(PKG_BUILDDIR)/.git-downloaded ; \
	} > /dev/null 2>&1 || true

distclean::
	rm -rf "$(PKG_BUILDDIR)"
