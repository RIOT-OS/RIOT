#
# This file allows specifying a local folder using PKG_SOURCE_LOCAL.
#
# Every clean or prepare will remove $(PKG_SOURCE_DIR) and copy over
# $(PKG_SOURCE_LOCAL).  This is intended to be used during package development.
#
# WARNING: any local changes made to $(PKG_SOURCE_DIR) *will* get lost!

.PHONY: prepare clean all FORCE

all: $(PKG_SOURCE_DIR)/.prepared

prepare: $(PKG_SOURCE_DIR)/.prepared
	@true

$(PKG_SOURCE_DIR)/.prepared:
	rm -Rf $(PKG_SOURCE_DIR)
	mkdir -p $$(dirname $(PKG_SOURCE_DIR))
	cp -a $(PKG_SOURCE_LOCAL) $(PKG_SOURCE_DIR)
	touch $@

clean::
	@rm -f $(PKG_SOURCE_DIR)/.prepared
