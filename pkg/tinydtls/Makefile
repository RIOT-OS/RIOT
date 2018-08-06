PKG_NAME=tinydtls
PKG_URL=https://git.eclipse.org/r/tinydtls/org.eclipse.tinydtls
PKG_VERSION=84f1f4e3ca13101a5a9aedeaf08039636c4f34dd
PKG_LICENSE=EPL-1.0,EDL-1.0

CFLAGS += -Wno-implicit-fallthrough
# following is require due to known issue with newlib 2.4.x, see bug report:
# http://lists-archives.com/cygwin/97008-gettimeofday-not-defined.html
CFLAGS += -D_XOPEN_SOURCE=600

.PHONY: all

all: git-download
	@cp $(PKG_BUILDDIR)/Makefile.riot $(PKG_BUILDDIR)/Makefile
	@cp $(PKG_BUILDDIR)/aes/Makefile.riot $(PKG_BUILDDIR)/aes/Makefile
	@cp $(PKG_BUILDDIR)/ecc/Makefile.riot $(PKG_BUILDDIR)/ecc/Makefile
	"$(MAKE)" -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk

ifeq (llvm,$(TOOLCHAIN))
  CFLAGS += -Wno-format-nonliteral
endif
