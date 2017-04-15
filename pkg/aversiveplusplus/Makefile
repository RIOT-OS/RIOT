PKG_NAME=aversiveplusplus
PKG_URL=https://github.com/AversivePlusPlus/AversivePlusPlus.git
PKG_VERSION=7189e63551fd156719528016d585d870996d8834
PKG_LICENSE=BSD-3-Clause

.PHONY: all

all: git-download
	(cd $(PKG_BUILDDIR) && git submodule update --init --recursive)

include $(RIOTBASE)/pkg/pkg.mk
