PKG_NAME=lora-serialization
PKG_URL=https://github.com/leandrolanzieri/lora-serialization
PKG_VERSION=a9e0ccfc82bb69a5d88ea24799bffa61dfd0e886
PKG_LICENSE=LGPLv2.1

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR) -f $(CURDIR)/Makefile.$(PKG_NAME)

include $(RIOTBASE)/pkg/pkg.mk
