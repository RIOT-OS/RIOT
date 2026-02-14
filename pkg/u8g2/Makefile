PKG_NAME=u8g2
PKG_URL=https://github.com/olikraus/u8g2
PKG_VERSION=764ee93150d23779ea9d8a7e8cae73efb60749d4 # v2.36.17
PKG_LICENSE=BSD-2-Clause

include $(RIOTBASE)/pkg/pkg.mk

all: $(filter u8g2_%,$(filter-out u8g2_csrc% u8g2_disp_dev, $(USEMODULE)))
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/csrc -f $(CURDIR)/Makefile.$(PKG_NAME)_csrc

u8g2_%:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/sys/$*/common -f $(CURDIR)/Makefile.$(PKG_NAME)_$*
