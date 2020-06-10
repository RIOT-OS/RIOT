PKG_NAME=tinydtls
PKG_URL=https://github.com/eclipse/tinydtls.git
PKG_VERSION=7a0420bfe3c041789cc0fe87822832f2fd12d0c3
PKG_LICENSE=EPL-1.0,EDL-1.0

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-implicit-fallthrough
# following is require due to known issue with newlib 2.4.x, see bug report:
# http://lists-archives.com/cygwin/97008-gettimeofday-not-defined.html
CFLAGS += -D_XOPEN_SOURCE=600

all:
	"$(MAKE)" -C $(PKG_BUILDDIR) -f $(PKG_BUILDDIR)/Makefile.riot
	"$(MAKE)" -C $(PKG_BUILDDIR)/aes -f $(PKG_BUILDDIR)/aes/Makefile.riot
	"$(MAKE)" -C $(PKG_BUILDDIR)/ecc -f $(PKG_BUILDDIR)/ecc/Makefile.riot

ifeq (llvm,$(TOOLCHAIN))
  CFLAGS += -Wno-format-nonliteral
endif
