PKG_NAME=cayenne-lpp
PKG_URL=https://github.com/aabadie/cayenne-lpp
PKG_VERSION=ad3aa26043e221eda28ac3da2484e1ef84986442
PKG_LICENSE=LGPLv2.1

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR) -f $(CURDIR)/Makefile.$(PKG_NAME)

include $(RIOTBASE)/pkg/pkg.mk
