#
# Include this file if your Package needs to be checked out by git
#
PKG_DIR?=$(CURDIR)
PKG_BUILDDIR?=$(BINDIR)/pkg/$(PKG_NAME)

.PHONY: git-download

git-download: $(PKG_BUILDDIR)/.git-downloaded

$(PKG_BUILDDIR)/.git-downloaded:
	mkdir -p $(PKG_BUILDDIR)
	$(GITCACHE) clone "$(PKG_URL)" "$(PKG_VERSION)" "$(PKG_BUILDDIR)"
	if test -d "$(PKG_DIR)"/patches; then \
		git -C "$(PKG_BUILDDIR)" am --ignore-whitespace "$(PKG_DIR)"/patches/*.patch; \
	fi
	touch $@

clean::
	@echo "Cleaning package $(PKG_NAME)..."
	rm -rf "$(PKG_BUILDDIR)"

distclean::
	rm -rf "$(PKG_BUILDDIR)"
