PKG_NAME=u8g2
PKG_URL=https://github.com/olikraus/u8g2
PKG_VERSION=2e47ff05e7e7b38057fa875e14c4ecce2fb6a615
PKG_LICENSE=BSD-2-Clause

include $(RIOTBASE)/pkg/pkg.mk

all: $(filter u8g2_%,$(filter-out u8g2_csrc%, $(USEMODULE)))
	"$(MAKE)" -C $(PKG_BUILDDIR)/csrc -f $(CURDIR)/Makefile.$(PKG_NAME)_csrc

u8g2_%:
	"$(MAKE)" -C $(PKG_BUILDDIR)/sys/$*/common -f $(CURDIR)/Makefile.$(PKG_NAME)_$*
