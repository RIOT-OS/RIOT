PKG_NAME=tinydtls
PKG_URL=https://github.com/rfuentess/TinyDTLS.git
# PKG_VERSION=RIOT-OS
PKG_VERSION=d06ad84d3e8bd3e86f7557faee34b68d5f8c0129
PKG_LICENSE=EPL-1.0,EDL-1.0

.PHONY: all

all: git-download
	$(MAKE) -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
