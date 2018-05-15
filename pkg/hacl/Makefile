PKG_NAME=hacl
PKG_URL=https://github.com/mitls/hacl-c
PKG_VERSION=aac05f5094fc92569169d5a2af54c12387160634
PKG_LICENSE=MIT

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR) -f $(CURDIR)/Makefile.$(PKG_NAME)

include $(RIOTBASE)/pkg/pkg.mk
