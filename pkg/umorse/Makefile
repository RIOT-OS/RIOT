PKG_NAME=umorse
PKG_URL=https://github.com/smlng/uMorse
PKG_VERSION=1dc14abdba22cca2f7efc053b2bce327bc7db97e
PKG_LICENSE=MIT

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -D_XOPEN_SOURCE=600

all:
	@cp Makefile.umorse $(PKG_BUILDDIR)/Makefile
	"$(MAKE)" -C $(PKG_BUILDDIR)
