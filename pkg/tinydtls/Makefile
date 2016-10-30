PKG_NAME=tinydtls
PKG_URL=git://github.com/rfuentess/TinyDTLS.git
# PKG_VERSION=RIOT-OS
PKG_VERSION=f824b5553a865c186a9b41236be03358f0c8feaf
PKG_BUILDDIR ?= $(BINDIRBASE)/pkg/$(BOARD)/$(PKG_NAME)

.PHONY: all

all: git-download
	$(MAKE) -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
