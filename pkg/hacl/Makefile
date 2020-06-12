PKG_NAME=hacl
PKG_URL=https://github.com/mitls/hacl-c
PKG_VERSION=aac05f5094fc92569169d5a2af54c12387160634
PKG_LICENSE=MIT

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -DKRML_NOUINT128 -Wno-unused-parameter

all:
	"$(MAKE)" -C $(PKG_BUILDDIR) -f $(RIOTBASE)/Makefile.base
