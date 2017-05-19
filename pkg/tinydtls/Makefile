PKG_NAME=tinydtls
PKG_URL=https://github.com/rfuentess/TinyDTLS.git
#Last tested commit:
PKG_VERSION=5b5ba4b3c7a79dda168103be4d119ca489f89c3c
# (Official) Branch with tested(?) updates before merging to remote master
#PKG_VERSION=RIOT-Tested-Upgrades

PKG_LICENSE=EPL-1.0,EDL-1.0

.PHONY: all

all: git-download
	@cp $(PKG_BUILDDIR)/Makefile.riot $(PKG_BUILDDIR)/Makefile
	@cp $(PKG_BUILDDIR)/aes/Makefile.riot $(PKG_BUILDDIR)/aes/Makefile
	@cp $(PKG_BUILDDIR)/ecc/Makefile.riot $(PKG_BUILDDIR)/ecc/Makefile
	@cp $(PKG_BUILDDIR)/sha2/Makefile.riot $(PKG_BUILDDIR)/sha2/Makefile
	$(MAKE) -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
