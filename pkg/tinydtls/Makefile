PKG_NAME=tinydtls
PKG_URL=https://github.com/rfuentess/TinyDTLS.git
PKG_VERSION=6b27f4dbffb1bf2aac994cd523be119bd8ac71b0
# (Official) Branch with tested(?) updates before merging to remote master
#PKG_VERSION=RIOT-Tested-Upgrades

PKG_LICENSE=EPL-1.0,EDL-1.0

.PHONY: all

all: git-download
	$(MAKE) -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
