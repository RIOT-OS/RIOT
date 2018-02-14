PKG_NAME=tinydtls
PKG_URL=https://github.com/rfuentess/TinyDTLS.git
# PKG_VERSION=RIOT-OS
PKG_VERSION=eb6f017ab451bb6cc4428b3e449955a76aeeba19
PKG_LICENSE=EPL-1.0,EDL-1.0

CFLAGS += -Wno-implicit-fallthrough

.PHONY: all

all: git-download
	@cp $(PKG_BUILDDIR)/Makefile.riot $(PKG_BUILDDIR)/Makefile
	@cp $(PKG_BUILDDIR)/aes/Makefile.riot $(PKG_BUILDDIR)/aes/Makefile
	@cp $(PKG_BUILDDIR)/ecc/Makefile.riot $(PKG_BUILDDIR)/ecc/Makefile
	@cp $(PKG_BUILDDIR)/sha2/Makefile.riot $(PKG_BUILDDIR)/sha2/Makefile
	"$(MAKE)" -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
