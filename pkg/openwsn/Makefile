PKG_NAME=openwsn
PKG_URL=https://github.com/openwsn-berkeley/openwsn-fw.git
PKG_VERSION=ff25e5d0ae5d344ed793a724d60532fb917bf1f8

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
