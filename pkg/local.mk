#
# This file allows specifying a local folder using PKG_SOURCE_LOCAL.
#
# Every clean or prepare will remove $(PKG_BUILDDIR) and copy over
# $(PKG_SOURCE_LOCAL).  This is intended to be used during package development.
#
# WARNING: any local changes made to $(PKG_BUILDDIR) *will* get lost!

.PHONY: prepare git-download clean

git-download:
	@true

prepare: $(PKG_BUILDDIR)/.prepared
	@true

$(PKG_BUILDDIR)/.prepared:
	rm -Rf $(PKG_BUILDDIR)
	mkdir -p $$(dirname $(PKG_BUILDDIR))
	cp -a $(PKG_SOURCE_LOCAL) $(PKG_BUILDDIR)
	touch $@

clean::
	@rm -f $(PKG_BUILDDIR)/.prepared
