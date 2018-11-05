PKG_NAME=lora-serialization
PKG_URL=https://github.com/leandrolanzieri/lora-serialization
PKG_VERSION=90e49e4acd119f20ad2ad7b98751b88b08f564d7
PKG_LICENSE=LGPLv2.1

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR) -f $(CURDIR)/Makefile.$(PKG_NAME)

include $(RIOTBASE)/pkg/pkg.mk
