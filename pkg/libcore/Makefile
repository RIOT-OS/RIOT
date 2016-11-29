PKG_NAME     = libcore
PKG_URL      = https://github.com/rust-lang/rust
PKG_VERSION  = $(shell ./version.sh)

.PHONY: all

all: $(BINDIR)/$(PKG_NAME).a
$(BINDIR)/$(PKG_NAME).a: $(BINDIR)/$(PKG_NAME).rlib
	$(AD)ln -fs $< $@

$(BINDIR)/$(PKG_NAME).rlib: git-download
	$(AD)rustc --crate-name core --crate-type lib \
		--target $(RUST_TARGET) -o $@ \
		$(PKG_BUILDDIR)/src/$(PKG_NAME)/lib.rs

include $(RIOTBASE)/Makefile.rust
include $(RIOTBASE)/pkg/pkg.mk
