PKG_NAME=openwsn-fw
PKG_URL=https://github.com/openwsn-berkeley/openwsn-fw.git
PKG_VERSION=dcf26e99b65ace4b1d8551a9962ee976fe7f0359
PKG_BUILDDIR ?= $(BINDIRBASE)/pkg/$(BOARD)/$(PKG_NAME)
PKG_LICENSE=BSD-3-Clause

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
