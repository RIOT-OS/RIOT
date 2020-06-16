PKG_NAME=cn-cbor
PKG_URL=https://github.com/cabo/cn-cbor
PKG_VERSION=f1cf9ffdf5cfab935a45900556f9b68af925c256
PKG_LICENSE=MIT

include $(RIOTBASE)/pkg/pkg.mk

# Enable code forcing aligned reads
CFLAGS += -DCBOR_ALIGN_READS

all:
	"$(MAKE)" -C $(PKG_BUILDDIR)/src -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_NAME)
