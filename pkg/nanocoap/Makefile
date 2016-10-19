PKG_NAME=nanocoap
PKG_URL=https://github.com/kaspar030/sock
PKG_VERSION=5649e325a3a9047b48ffbbcc283f7a4558978737

.PHONY: all

all: git-download
	@cp Makefile.nanocoap $(PKG_BUILDDIR)/nanocoap/Makefile
	"$(MAKE)" -C $(PKG_BUILDDIR)/nanocoap

include $(RIOTBASE)/pkg/pkg.mk
