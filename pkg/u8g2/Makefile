PKG_NAME=u8g2
PKG_URL=https://github.com/olikraus/u8g2
PKG_VERSION=3b8939cd015d6aca6a19f761df84859916f1f7d9 # approximately v2.36.18
PKG_LICENSE=BSD-2-Clause

include $(RIOTBASE)/pkg/pkg.mk

all: $(filter u8g2_%,$(filter-out u8g2_csrc% u8g2_disp_dev, $(USEMODULE)))
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/csrc -f $(CURDIR)/Makefile.$(PKG_NAME)_csrc

u8g2_%:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/sys/$*/common -f $(CURDIR)/Makefile.$(PKG_NAME)_$*
