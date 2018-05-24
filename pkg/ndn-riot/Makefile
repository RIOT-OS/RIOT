PKG_NAME=ndn-riot
PKG_URL=https://github.com/named-data-iot/ndn-riot
PKG_VERSION=34c5eb8adf198049f0a56048825b505c561a8874
PKG_LICENSE=LGPLv2.1

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
