PKG_NAME     = libcore
CRATE_NAME   = core
PKG_URL      = https://github.com/rust-lang/rust
PKG_VERSION  = $(shell ./version.sh)

USEPKG    =
USEMODULE =

.PHONY: all

all: $(BINDIR)/rust/$(PKG_NAME).a

include $(RIOTBASE)/pkg/rust/Makefile.crate
